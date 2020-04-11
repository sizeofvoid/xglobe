/*
 * renderer.cpp
 *
 * $Id: renderer.cpp,v 1.5 2000/06/19 01:03:53 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 * Copyright (C) 2020 Rafael Sadowski
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: renderer.cpp,v $
 * Revision 1.5  2000/06/19 01:03:53  espie
 * Fix stupid bug that yielded greenish hue.
 * Let city lights show through cloud cover.
 *
 * Revision 1.4  2000/06/17 13:03:45  espie
 * Move std::min/std::max to compute.h
 * Move solve_conflicts to its own function
 * Compute interpolated pixel more efficiently.
 *
 * Revision 1.3  2000/06/17 12:03:10  espie
 * Autocompute ambient light for a night map.
 * fix typo
 *
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.9  1999/12/06 16:13:26  thorsten
 * -added -rot switch
 *
 * Revision 0.8  1999/12/05 19:18:30  thorsten
 * by Andrew:
 * - added cloud layer rendering
 *
 * Revision 0.7  1999/09/19 19:45:53  thorsten
 * by Andrew Sumner:
 * - added -shade_area switch
 *
 * Revision 0.6  1999/08/06 16:14:46  thorsten
 * Forgot to initialize a variable, which could cause a segfault.
 *
 * Revision 0.5  1999/07/19 12:44:39  thorsten
 * - works with QT 2
 * - added -term switch
 * - nicer output when using night maps
 *
 * Revision 0.4  1999/07/13 17:40:02  thorsten
 * 8 bit images can now be used without increasing colordepth to 32 bit
 * can now draw stars in the background
 * by Andrew Sumner:
 * can now display a background image
 * globe does no longer need to be centered
 *
 * Revision 0.2  1999/01/08 17:25:28  thorsten
 * added grid drawing code
 *
 * Revision 0.2  1999/01/06 13:21:33  thorsten
 * added optional display of a grid
 *
 * Revision 0.1  1998/12/10 20:08:57  thorsten
 * initial revision
 *
 */

#include "renderer.h"
#include "compute.h"
#include "file.h"
#include "sunpos.h"
#include <math.h>
#include <QApplication>
#include <QDateTime>
#include <QPainter>
#include <QRgba64>
#include <QPixmap>
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------------*/

Renderer::Renderer(const QSize& size, const QString& mapfile)
{
    markerlist = nullptr;
    map = nullptr;
    mapnight = nullptr;
    backImage = nullptr;
    mapcloud = nullptr;
    track_clouds = nullptr;

    renderedImage = new QImage(size, QImage::Format_RGB32);
    map = loadImage(!mapfile.isEmpty() ? mapfile : default_map);

     fprintf(stderr, "Map size: %dx%d\n", map->width(), map->height());

    this->radius = 1000.;
    this->view_long = 0.;
    this->view_lat = 0.;
    this->sun_long = 0.;
    this->sun_lat = 0.;
    this->fov = 0.5 * M_PI / 180.;
    this->zoom = 0.9;
    this->ambientRed = 0.15;
    this->ambientGreen = 0.15;
    this->ambientBlue = 0.15;
    this->show_label = true;
    this->gridtype = GridType::no;
    this->d_gridline = 15.0 * M_PI / 180.;
    this->d_griddot = M_PI / 180.;
    stars = nullptr;
    this->trans = 0.0;
    this->rot = 0.0;

    calcDistance();
}

/* ------------------------------------------------------------------------*/

QImage* Renderer::loadImage(const QString& name)
{
    QImage* m = new QImage();

    if (!m->load(find_xglobefile(name))) {
        fprintf(stderr, "Error while opening map \"%s\"!\n", name.toLatin1().data());
        ::exit(22);
    }

    if (m->depth() < 8)
        *m = m->convertToFormat(QImage::Format_Indexed8);

    return m;
}

/* ------------------------------------------------------------------------*/

int Renderer::loadNightMap(const QString& nmapfile)
{
    if (!mapnight) // we already have a night map!
        return 1;

    mapnight = loadImage(!nmapfile.isEmpty() ? nmapfile : default_map_night);

    return 1;
}

/* ------------------------------------------------------------------------*/

static inline bool bad_color(int r, int g, int b)
{
    return (r == 255 && b == 255) || (r == 2 && g == 2 && b == 2);
}

int Renderer::loadCloudMap(const QString& cmapfile, int cf)
{
    if (track_clouds == nullptr && !cmapfile.isEmpty()) {
        /* create scale array, atan looks fine to sharpen clouds */
        for (int i = 0; i < 255; i++) {
            int j = atan((i - cf) / 20.0) * 290 / M_PI + 125;
            if (j < 0)
                v[i] = 0;
            else if (j > 255)
                v[i] = 255;
            else
                v[i] = j;
        }
        track_clouds = new FileChange(find_xglobefile(cmapfile));
    }

    if (track_clouds == nullptr || !track_clouds->reload())
        return 1;
    if (mapcloud)
        delete mapcloud;
    mapcloud = loadImage(track_clouds->name());
    if (!mapcloud) {
        //fprintf(stderr, "Error loading cloud mapfile \"%s\"\n", cmapfile);
        ::exit(21);
    }
    int endy = mapcloud->height();
    int endx = mapcloud->width();

    if (mapcloud->depth() == 8)
        *mapcloud = mapcloud->convertToFormat(QImage::Format_RGB32);

    int sb, sg, sr;
    QRgb* p1;

    /* filter out the pink continent outlines */
    for (int py = 0; py < endy; py++) {
        p1 = scan32(*mapcloud, 0, py);
        for (int px = 0; px < endx; px++) {
            sr = qRed(*p1);
            sg = qGreen(*p1);
            sb = qBlue(*p1);

            if (bad_color(sr, sg, sb)) {
                int x = px;
                int y = py;
                /* use a random walk to grab near data */
                do {
                    switch (gen(4)) {
                    case 0:
                        if (y < endy - 1) {
                            y++;
                            break;
                        }
                    case 1:
                        if (y > 0) {
                            y--;
                            break;
                        }
                    case 2:
                        x++;
                        if (x == endx)
                            x = 0;
                        break;
                    case 3:
                        x--;
                        if (x < 0)
                            x += endx;
                        break;
                    }
                    QRgb* q1 = scan32(*mapcloud, x, y);
                    sr = qRed(*q1);
                    sg = qGreen(*q1);
                    sb = qBlue(*q1);
                } while (bad_color(sr, sg, sb));
                *p1 = qRgb(sr, sg, sb);
            }

            p1++;
        }
    }

    for (int py = 0; py < endy; py++) {
        p1 = scan32(*mapcloud, 0, py);
        for (int px = 0; px < endx; px++) {
            sb = qBlue(*p1);
            sg = qGreen(*p1);
            sr = qRed(*p1);
            *p1 = qRgb(v[sr], v[sg], v[sb]);
            p1++;
        }
    }
    return 1;
}

/* ------------------------------------------------------------------------*/

int Renderer::loadBackImage(const QString& imagefile, bool tld)
{
    if (backImage != nullptr)
        return 1;

    tiled = tld;

    backImage = loadImage(!imagefile.isEmpty() ? imagefile : default_map_back);

    if (!tiled) {
        QSize smallSize (renderedImage->width(), renderedImage->height());
        QImage bi = backImage->scaled(smallSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        delete backImage;
        backImage = new QImage(bi);
    }

    return 1;
}

/* ------------------------------------------------------------------------*/

Renderer::~Renderer()
{
    delete renderedImage;
    delete map;
    delete mapnight;
    delete mapcloud;
    delete backImage;
    delete track_clouds;
}

/* ------------------------------------------------------------------------*/

void Renderer::setViewPos(double lat, double lon)
{
    while (lat >= 360.)
        lat -= 360.;
    while (lat <= -360.)
        lat += 360.;
    if (lat > 90.) {
        lat = 90. - (lat - 90.);
        lon += 180.;
    }
    if (lat < -90.) {
        lat = -90. + (-lat - 90.);
        lon += 180.;
    }

    while (lon >= 360.)
        lon -= 360.;
    while (lon <= -360.)
        lon += 360.;
    if (lon > 180.)
        lon = -180. + (lon - 180.);
    if (lon < -180.)
        lon = 180. + (lon + 180.);

    view_lat = lat * M_PI / 180.;
    view_long = lon * M_PI / 180.;
}

/* ------------------------------------------------------------------------*/

double Renderer::getViewLat()
{
    return view_lat * 180. / M_PI;
}

/* ------------------------------------------------------------------------*/

double Renderer::getViewLong()
{
    return view_long * 180. / M_PI;
}

/* ------------------------------------------------------------------------*/

void Renderer::setRotation(double r)
{
    rot = r * M_PI / 180.;
}

/* ------------------------------------------------------------------------*/

double Renderer::getRotation()
{
    return rot * 180. / M_PI;
}

/* ------------------------------------------------------------------------*/

double Renderer::getSunLat()
{
    return sun_lat * 180. / M_PI;
}

/* ------------------------------------------------------------------------*/

double Renderer::getSunLong()
{
    return sun_long * 180. / M_PI;
}

/* ------------------------------------------------------------------------*/

void Renderer::setZoom(double z)
{
    zoom = z;
    calcDistance();
}

/* ------------------------------------------------------------------------*/

double Renderer::getZoom()
{
    return zoom;
}

/* ------------------------------------------------------------------------*/

void Renderer::setMarkerList(MarkerList* l)
{
    markerlist = l;
}

/* ------------------------------------------------------------------------*/

void Renderer::showLabel(bool show)
{
    show_label = show;
}

/* ------------------------------------------------------------------------*/

void Renderer::setShift(int x, int y)
{
    shift_x = x;
    shift_y = y;
}

/* ------------------------------------------------------------------------*/

int Renderer::getShiftX()
{
    return shift_x;
}

/* ------------------------------------------------------------------------*/

int Renderer::getShiftY()
{
    return shift_y;
}

/* ------------------------------------------------------------------------*/

void Renderer::setLabelPos(int x, int y)
{
    label_x = x;
    label_y = y;
}

/* ------------------------------------------------------------------------*/

void Renderer::setShadeArea(double area)
{
    shade_area = area;
}

/* ------------------------------------------------------------------------*/

void Renderer::setAmbientRGB(QRgba64 const& rgb)
{
    const int samples = 100;
    if (mapnight != nullptr) {
        // Auto-calibrate ambient rgb by random sampling.
        int dr_tot = 0, dg_tot = 0, db_tot = 0;
        int nr_tot = 0, ng_tot = 0, nb_tot = 0;
        for (int i = 0; i < samples; i++) {
            double longitude = gen(3600) * M_PI / 1800.0;
            double latitude = gen(1800) * M_PI / 1800.0 - M_PI / 2.0;
            int r, g, b;
            getMapColorLinear(map, longitude, latitude, &r, &g, &b);
            dr_tot += r;
            dg_tot += g;
            db_tot += b;
            getMapColorLinear(mapnight, longitude, latitude, &r, &g, &b);
            nr_tot += r;
            ng_tot += g;
            nb_tot += b;
        }
        ambientRed = ((double)nr_tot) / dr_tot;
        ambientGreen = ((double)ng_tot) / dg_tot;
        ambientBlue = ((double)nb_tot) / db_tot;
    }
    else {
        ambientRed = rgb.red();
        ambientGreen = rgb.green();
        ambientBlue = rgb.blue();
    }
}

/* ------------------------------------------------------------------------*/

void Renderer::setFov(double fov)
{
    this->fov = fov * M_PI / 180.;
    calcDistance();
}

/* ------------------------------------------------------------------------*/

void Renderer::setTime(time_t t)
{
    time_to_render = t;
    calcLightVector(); // calc. current sun position
}

/* ------------------------------------------------------------------------*/

time_t Renderer::getTime()
{
    return time_to_render;
}

/* ------------------------------------------------------------------------*/

void Renderer::setNumGridLines(int num)
{
    d_gridline = M_PI / (2.0 * num);
}

/* ------------------------------------------------------------------------*/

int Renderer::getNumGridLines()
{
    return (int)(M_PI / 2 * d_gridline);
}

/* ------------------------------------------------------------------------*/

void Renderer::setNumGridDots(int num)
{
    d_griddot = 2.0 * M_PI / num;
}

/* ------------------------------------------------------------------------*/

int Renderer::getNumGridDots()
{
    return (int)(2.0 * M_PI / d_griddot);
}

/* ------------------------------------------------------------------------*/

void Renderer::setGridType(GridType type)
{
    gridtype = type;
}

/* ------------------------------------------------------------------------*/

GridType Renderer::getGridType()
{
    return gridtype;
}

/* ------------------------------------------------------------------------*/

void Renderer::setTransition(double t)
{
    trans = t;
    if (trans >= 1.0)
        trans = 0.9999;
    else if (trans < 0.0)
        trans = 0.0;
}

/* ------------------------------------------------------------------------*/

double Renderer::getTransition()
{
    return trans;
}

/* ------------------------------------------------------------------------*/

void Renderer::calcDistance()
{
    double x;
    double tan_a;

    // distance of camera to projection plane
    proj_dist = std::min(renderedImage->width(), renderedImage->height()) / tan(fov);

    x = zoom * std::min(renderedImage->width(), renderedImage->height()) / 2.;
    tan_a = x / proj_dist;
    // distance of camera camera to center of earth ( = coordinate origin)
    center_dist = radius / sin(atan(tan_a));
}

/* ------------------------------------------------------------------------*/

void Renderer::renderFrame()
{
    double dir_x, dir_y, dir_z; // direction of cast ray
    double hit_x, hit_y, hit_z; // hit position on earth surface
    double hit2_x, hit2_y, hit2_z; // mirrored hit position on earth surface
    double sp_x, sp_y, sp_z; // intersection point of globe and ray
    double a, b, c; // coeff. of quadratic equation
    double radikand;
    double wurzel;
    double r; // r'
    double radiusq = radius * radius;
    double s1, s2, s; // distance between intersections and
        // camera position
    double longitude, latitude; // coordinates of hit position
    double light_angle; // cosine of angle between sunlight and
        // surface normal
    int startx, endx; // the region to be painted
    int starty, endy;
    int real_startx, real_endx;
    int temp;

    QRgb* p; // pointer to current pixel
    QRgb* q;

    loadCloudMap(); // reload cloudmap, if changed
    int half_width = renderedImage->width() / 2 + renderedImage->width() % 2 - 1;

    // clear image
    for (int i = 0; i < renderedImage->height(); i++) {
        p = scan32(*renderedImage, 0, i);
        memset(p, 0, renderedImage->bytesPerLine());
    }

    if (backImage != nullptr)
        copyBackImage();

    drawStars();

    // rotation matrix
    RotMatrix mat(rot, view_long, view_lat);

    dir_z = -proj_dist;

    // indifferent coeff.
    c = center_dist * center_dist - radiusq;

    // calc. radius of projected sphere
    b = 2 * center_dist * dir_z;
    radius_proj = (int)sqrt(b * b / (4 * c) - dir_z * dir_z);

    startx = (renderedImage->width() / 2 - radius_proj - 1);
    startx = (startx < 0) ? 0 : startx;
    endx = renderedImage->width() - startx - 1;
    starty = (renderedImage->height() / 2 - radius_proj - 1);
    starty = (starty < 0) ? 0 : starty;
    endy = renderedImage->height() - starty - 1;

    for (int py = starty; py <= endy; py++) {
        // handle any paint events waiting in the queue
        qApp->processEvents();

        temp = radius_proj * radius_proj - (py - renderedImage->height() / 2) * (py - renderedImage->height() / 2);

        if (temp >= 0)
            startx = (renderedImage->width() / 2 - (int)sqrt(temp));
        else
            startx = (renderedImage->width() / 2);

        startx = (startx < 0) ? 0 : startx;
        endx = renderedImage->width() - startx - 1;

        // calculate offset into image data
        if (py + shift_y < 0 || py + shift_y >= renderedImage->height())
            continue;
        real_startx = startx + shift_x;
        real_startx = (real_startx < 0 ? 0 : real_startx);
        real_startx = (real_startx >= renderedImage->width() ? renderedImage->width() - 1 : real_startx);

        real_endx = endx + shift_x;
        real_endx = (real_endx < 0 ? 0 : real_endx);
        real_endx = (real_endx >= renderedImage->width() ? renderedImage->width() - 1 : real_endx);

        p = scan32(*renderedImage, real_startx, py + shift_y);
        q = scan32(*renderedImage, real_endx, py + shift_y);

        if (rot == 0.) // optimization when using no rotation
            endx = half_width;

        for (int px = startx; px <= endx; px++) {
            dir_x = (px - renderedImage->width() / 2);
            dir_y = (-py + renderedImage->height() / 2);

            a = dir_x * dir_x + dir_y * dir_y + dir_z * dir_z;
            b = 2 * center_dist * dir_z;
            // c constant, see above

            radikand = b * b - 4 * a * c; // what's under the sq.root when solving the
                // quadratic equation
            if (radikand >= 0.) // solution exists <=> intersection
            {
                wurzel = sqrt(radikand);
                s1 = (-b + wurzel) / (2. * a);
                s2 = (-b - wurzel) / (2. * a);
                s = (s1 < s2) ? s1 : s2; // smaller solution belongs to nearer
                    // intersection
                sp_x = s * dir_x; // sp = camera pos + s*dir
                sp_y = s * dir_y;
                sp_z = center_dist + s * dir_z;

                mat.transform(sp_x, sp_y, sp_z, hit_x, hit_y, hit_z);

                if (rot == 0.) // optimization when using no rotation
                    mat.transform(-sp_x, sp_y, sp_z, hit2_x, hit2_y, hit2_z);

                longitude = atan(hit_x / hit_z);
                if (hit_z < 0.)
                    longitude = M_PI + longitude;

                r = (double)sqrt(hit_x * hit_x + hit_z * hit_z);
                latitude = atan(-hit_y / r);

                light_angle = (light_x * hit_x + light_y * hit_y + light_z * hit_z) / radius;
                light_angle = pow(light_angle, 1.0 - trans);

                // Set pixel in image
                *p++ = getPixelColor(longitude, latitude, light_angle);

                // only when using no rotation:
                // mirror the left half-circle of the globe: we need a new position
                // and have to recalculate the light intensity
                if (rot == 0.) {
                    light_angle = (light_x * hit2_x + light_y * hit2_y + light_z * hit2_z) / radius;
                    light_angle = pow(light_angle, 1.0 - trans);
                    *q-- = getPixelColor(2 * view_long - longitude, latitude, light_angle);
                }
            }
            else {
                p++;
                q--;
            }
        }
    }

    if (gridtype != GridType::no)
        drawGrid();

    if (markerlist)
        drawMarkers();

    //if (show_label)
     ///   drawLabel();
}

/* ------------------------------------------------------------------------*/

void Renderer::copyBackImage()
{
    QRgb *p, *bp;
    unsigned char* c_bp;
    unsigned int y, x, by, bx;
    unsigned int mywidth = renderedImage->width(), myheight = renderedImage->height();
    unsigned int bwidth = backImage->width(), bheight = backImage->height();

    for (y = 0, by = 0; y < myheight; y++, by++) {
        if (by >= bheight)
            by = 0;

        p = scan32(*renderedImage, 0, y);

        if (backImage->depth() == 32) {
            bp = scan32(*backImage, 0, by);
            for (x = 0, bx = 0; x < mywidth; x++, bx++) {
                if (bx >= bwidth) {
                    bx = 0;
                    bp = scan32(*backImage, 0, by);
                }
                *p++ = *bp++;
            }
        }
        else {
            c_bp = (unsigned char*)backImage->scanLine(by);
            for (x = 0, bx = 0; x < mywidth; x++, bx++) {
                if (bx >= bwidth) {
                    bx = 0;
                    c_bp = (unsigned char*)backImage->scanLine(by);
                }
                *p++ = backImage->color(*c_bp++);
            }
        }
    }
}

/* ------------------------------------------------------------------------*/

void Renderer::calcLightVector()
{
    SunPos::GetSunPos(time_to_render, &sun_lat, &sun_long);

    light_x = cos(sun_lat) * sin(sun_long);
    light_y = sin(sun_lat);
    light_z = cos(sun_lat) * cos(sun_long);
}

/* ------------------------------------------------------------------------*/

unsigned int Renderer::getPixelColor(double longitude, double latitude,
    double angle)
{
    int r, g, b;
    double shade_angle;

    if (shade_area)
        shade_angle = angle / shade_area;
    else
        shade_angle = 1.0;

    if (mapnight != nullptr) {
        if (angle > shade_area) {
            getMapColorLinear(map, longitude, latitude, &r, &g, &b);
        }
        else if (angle < -0.1) {
            getMapColorLinear(mapnight, longitude, latitude, &r, &g, &b);
        }
        else if (angle > 0.1) {
            getMapColorLinear(map, longitude, latitude, &r, &g, &b);
            r = r * (ambientRed + shade_angle * (1. - ambientRed));
            g = g * (ambientGreen + shade_angle * (1. - ambientGreen));
            b = b * (ambientBlue + shade_angle * (1. - ambientBlue));
        }
        else {
            double x;
            int nr, ng, nb; // rgb values of night pixel

            getMapColorLinear(map, longitude, latitude, &r, &g, &b);
            getMapColorLinear(mapnight, longitude, latitude, &nr, &ng, &nb);
            x = -5.0 * angle + 0.5;
            if (angle > 0.) {
                r = x * nr + (1.0 - x) * r * (ambientRed + shade_angle * (1. - ambientRed));
                g = x * ng + (1.0 - x) * g * (ambientGreen + shade_angle * (1. - ambientGreen));
                b = x * nb + (1.0 - x) * b * (ambientBlue + shade_angle * (1. - ambientBlue));
            }
            else {
                r = x * nr + (1.0 - x) * r * ambientRed;
                g = x * ng + (1.0 - x) * g * ambientGreen;
                b = x * nb + (1.0 - x) * b * ambientBlue;
            }
        }
    }
    else {
        getMapColorLinear(map, longitude, latitude, &r, &g, &b);
        if (angle < shade_area && angle > 0.) {
            r *= ambientRed + shade_angle * (1. - ambientRed);
            g *= ambientGreen + shade_angle * (1. - ambientGreen);
            b *= ambientBlue + shade_angle * (1. - ambientBlue);
        }
        else if (angle < 0.) {
            r *= ambientRed;
            g *= ambientGreen;
            b *= ambientBlue;
        }
    }

    // correct luminosity for clouds
    if (mapcloud != nullptr) {
        int cr, cg, cb;
        getMapColorLinear(mapcloud, longitude, latitude, &cr, &cg, &cb);
        if (cr >= 0) {
            int ar, ag, ab;
            // compute ambient light value
            ar = ag = ab = 256;
            if (angle > 0.0 && angle < shade_area) {
                ar *= (ambientRed + shade_angle * (1.0 - ambientRed));
                ag *= (ambientGreen + shade_angle * (1.0 - ambientGreen));
                ab *= (ambientBlue + shade_angle * (1.0 - ambientBlue));
            }
            else if (angle <= 0.0) {
                ar *= ambientRed;
                ag *= ambientGreen;
                ab *= ambientBlue;
            }
            if (r > ar && g > ag && b > ab) {
                cr /= 2;
                cg /= 2;
                cb /= 2;
            }
#if 0
      if (r > ar)
      	cr /= 2;
      if (g > ag);
      	cg /= 2;
      if (b > ab)
      	cb /= 2;
#endif
            // create opacity depending on ambient light
            r = (ar * cr + r * (256 - cr)) / 256;
            g = (ag * cg + g * (256 - cg)) / 256;
            b = (ab * cb + b * (256 - cb)) / 256;
        }
    }
    return qRgb(r, g, b);
}

/* ------------------------------------------------------------------------*/

void Renderer::getMapColorLinear(QImage* m, double longitude, double latitude,
    int* r, int* g, int* b)
{
    latitude += M_PI / 2;
    longitude += M_PI;
    double posx = longitude * m->width() / (2 * M_PI);
    double posy = latitude * m->height() / M_PI;

    if (posy >= m->height()) {
        posy = 2 * m->height() - posy;
        posx += m->width() / 2;
    }
    else if (posy < 0) {
        posy = -posy;
        posx += m->width() / 2;
    }
    while (posx >= m->width())
        posx -= m->width();
    while (posx < 0)
        posx += m->width();

    int x11 = (int)posx;
    int y1 = (int)posy;
    int x12 = x11 + 1;
    if (x12 == m->width())
        x12 = 0;
    int x21 = x11;
    int y2 = y1 + 1;
    if (y2 == m->height()) {
        y2--;
        x21 = x11 + m->width() / 2;
        if (x21 >= m->width())
            x21 -= m->width();
    }
    int x22 = x12;
    double dx = posx - x11;
    double dy = posy - y1;

    QRgb c11, c12, c21, c22;

    // offset into map pixel data
    if (m->depth() == 32) {
        QRgb* p = scan32(*m, 0, y1);
        c11 = p[x11];
        c12 = p[x12];
        p = scan32(*m, 0, y2);
        c21 = p[x21];
        c22 = p[x22];
    }
    else //m->depth() == 8
    {
        unsigned char* p = (unsigned char*)m->scanLine(y1);
        c11 = m->color(p[x11]);
        c12 = m->color(p[x12]);
        p = (unsigned char*)m->scanLine(y2);
        c21 = m->color(p[x21]);
        c22 = m->color(p[x22]);
    }

    *r = (int)((qRed(c22) * dx + qRed(c21) * (1 - dx)) * dy + (qRed(c12) * dx + qRed(c11) * (1 - dx)) * (1 - dy));
    *g = (int)((qGreen(c22) * dx + qGreen(c21) * (1 - dx)) * dy + (qGreen(c12) * dx + qGreen(c11) * (1 - dx)) * (1 - dy));
    *b = (int)((qBlue(c22) * dx + qBlue(c21) * (1 - dx)) * dy + (qBlue(c12) * dx + qBlue(c11) * (1 - dx)) * (1 - dy));
}

/* ------------------------------------------------------------------------*/

void Renderer::drawMarkers()
{
    // Matrix M of renderFrame, but transposed
    RotMatrix mat(rot, view_long, view_lat, radius);
    mat.transpose();
    markerlist->render(mat, *renderedImage, radius, center_dist, proj_dist,
        shift_x, shift_y);
}

/* ------------------------------------------------------------------------*/

/* ------------------------------------------------------------------------*/

void Renderer::drawGrid()
{
    double lon, lat;
    double s_x, s_y, s_z;
    double loc_x, loc_y, loc_z;
    int screen_x, screen_y;
    double visible_angle;
    double cos_angle;
    double light_angle;
    double temp;

    QRgb* p;
    unsigned int pixel;
    int r, g, b;

    // Matrix M of renderFrame, but transposed
    RotMatrix mat(rot, view_long, view_lat, radius);
    mat.transpose();

    visible_angle = radius / center_dist;

    temp = M_PI / 2.0 - d_gridline;

    for (lat = -temp; lat <= temp + 0.01; lat += d_gridline) {
        s_y = sin(lat);

        for (lon = -M_PI; lon < M_PI; lon += d_griddot) {
            s_x = cos(lat) * sin(lon);
            s_z = cos(lat) * cos(lon);
            mat.transform(s_x, s_y, s_z, loc_x, loc_y, loc_z);

            cos_angle = loc_z / radius;
            light_angle = light_x * s_x + light_y * s_y + light_z * s_z;

            if (cos_angle < visible_angle)
                // location lies on the other side
                continue;

            loc_z = center_dist - loc_z;
            screen_x = (int)(loc_x * proj_dist / loc_z);
            screen_y = (int)(-loc_y * proj_dist / loc_z);
            screen_x += renderedImage->width() / 2 + shift_x;
            screen_y += renderedImage->height() / 2 + shift_y;

            if ((screen_x < 0) || (screen_x >= renderedImage->width()))
                // location out of bounds
                continue;
            if ((screen_y < 0) || (screen_y >= renderedImage->height()))
                continue;

            p = scan32(*renderedImage, screen_x, screen_y);

            // Set pixel in image
            if (gridtype == GridType::no) {
                pixel = getPixelColor(lon, -lat, light_angle);
                r = qRed(pixel) * 3;
                g = qGreen(pixel) * 3;
                b = qBlue(pixel) * 3;

                r = (r > 255) ? 255 : r;
                g = (g > 255) ? 255 : g;
                b = (b > 255) ? 255 : b;

                *p = qRgb(r, g, b);
            }
            else
                *p = qRgb(255, 255, 255);
        }
    }

    for (lon = -M_PI; lon < M_PI; lon += d_gridline) {
        for (lat = -temp; lat <= temp; lat += d_griddot) {
            s_x = cos(lat) * sin(lon);
            s_y = sin(lat);
            s_z = cos(lat) * cos(lon);
            mat.transform(s_x, s_y, s_z, loc_x, loc_y, loc_z);

            cos_angle = loc_z / radius;
            light_angle = light_x * s_x + light_y * s_y + light_z * s_z;

            if (cos_angle < visible_angle)
                // location lies on the other side
                continue;

            loc_z = center_dist - loc_z;
            screen_x = (int)(loc_x * proj_dist / loc_z);
            screen_y = (int)(-loc_y * proj_dist / loc_z);
            screen_x += renderedImage->width() / 2 + shift_x;
            screen_y += renderedImage->height() / 2 + shift_y;

            if ((screen_x < 0) || (screen_x >= renderedImage->width()))
                // location out of bounds
                continue;
            if ((screen_y < 0) || (screen_y >= renderedImage->height()))
                continue;

            p = scan32(*renderedImage, screen_x, screen_y);

            // Set pixel in image
            if (gridtype == GridType::nice) {
                pixel = getPixelColor(lon, -lat, light_angle);
                r = qRed(pixel) * 3;
                g = qGreen(pixel) * 3;
                b = qBlue(pixel) * 3;

                r = (r > 255) ? 255 : r;
                g = (g > 255) ? 255 : g;
                b = (b > 255) ? 255 : b;

                *p = qRgb(r, g, b);
            }
            else
                *p = qRgb(255, 255, 255);
        }
    }
}

/* ------------------------------------------------------------------------*/

QImage* Renderer::getImage()
{
    QImage* clonedImage = nullptr;

    clonedImage = new QImage(*renderedImage);
    assert(clonedImage != nullptr);
    return clonedImage;
}

/* ------------------------------------------------------------------------*/

void Renderer::drawLabel()
{
    QDateTime dt;
    QString labelstring;
    QPainter p;
    QColor transparentcolor = Qt::black;
    QColor whitecolor = Qt::white;
    unsigned int pixel;
    int x, y;
    int wx, wy;
    QRgb *src, *dest;
    double vlon, vlat;
    double slon, slat;
    struct tm* tm;

    dt.setTime_t(time_to_render);
    tm = localtime(&time_to_render);

    vlon = view_long * 180. / M_PI;
    vlat = view_lat * 180. / M_PI;
    slon = sun_long * 180. / M_PI;
    slat = sun_lat * 180. / M_PI;

    /*
    labelstring.sprintf("%s, %s %d. %d, %d:%02d %s\n"
                        "View pos %2.2f° %c %2.2f° %c\n"
                        "Sun pos %2.2f° %c %2.2f° %c",
#if QT_VERSION >= 200
        dt.date().toString(QLatin1String("dd")).data()->toLatin1(),
        dt.date().toString(QLatin1String("mm")).data()->toLatin1(),
#else
        dt.date().longDayName(dt.date().dayOfWeek()),
        dt.date().monthName(dt.date().month()),
#endif
        dt.date().day(), dt.date().year(),
        dt.time().hour(), dt.time().minute(),
        tzname[tm->tm_isdst],
        fabs(vlat), (vlat < 0.) ? 'S' : 'N',
        fabs(vlon), (vlon < 0.) ? 'W' : 'E',
        fabs(slat), (slat < 0.) ? 'S' : 'N',
        fabs(slon), (slon < 0.) ? 'W' : 'E');
        */

    QFont labelFont("helvetica", 12, QFont::Bold);
    QFontMetrics fm(labelFont);

#if QT_VERSION >= 200
    QRect br = fm.boundingRect(0, 0, 0, 0, Qt::AlignLeft | Qt::AlignTop,
        labelstring);
#else
    QRect br = fm.boundingRect(0, 0, 0, 0, AlignLeft | AlignTop, labelstring);
#endif
    QPixmap pm(br.width() + 10, br.height() + 10);

    p.begin(&pm);
    p.setFont(labelFont);
    p.fillRect(0, 0, pm.width(), pm.height(), transparentcolor);
    p.setPen(whitecolor);
#if QT_VERSION >= 200
    p.drawText(5, 5, br.width(), br.height(), Qt::AlignLeft | Qt::AlignTop,
        labelstring);
#else
    p.drawText(5, 5, br.width(), br.height(), AlignLeft | AlignTop, labelstring);
#endif
    p.end();

    QImage labelimage = pm.toImage();
    if (labelimage.depth() != 32)
        labelimage = labelimage.convertToFormat(QImage::Format_RGB32);

    if (label_x > 0)
        x = label_x;
    else
        x = renderedImage->width() - labelimage.width() + label_x;
    if (label_y > 0)
        y = label_y;
    else
        y = renderedImage->height() - labelimage.height() + label_y;

    for (wy = 0; wy < labelimage.height(); wy++) {
        dest = scan32(*renderedImage, x, y + wy);
        src = scan32(labelimage, 0, wy);

        for (wx = 0; wx < labelimage.width(); wx++) {
            if (QColor(*src) == whitecolor)
                *dest++ = *src++;
            else if (QColor(*src) == transparentcolor) {
                pixel = *dest;
                src++;
                *dest++ = qRgb(qRed(pixel) / 2, qGreen(pixel) / 2, qBlue(pixel) / 2);
            }
            else {
                pixel = *dest;
                int percent = qRed(*src++);
                *dest++ = qRgb(
                    (qRed(pixel) / 2 * (256 - percent) + 255 * percent) / 256,
                    (qGreen(pixel) / 2 * (256 - percent) + 255 * percent) / 256,
                    (qBlue(pixel) / 2 * (256 - percent) + 255 * percent) / 256);
            }
        }
    }
}

/* ------------------------------------------------------------------------*/

void Renderer::setStars(double f, bool show)
{
    if (stars)
        delete stars;
    if (show)
        stars = new Stars(f, *renderedImage);
    else
        stars = nullptr;
}

void Renderer::drawStars()
{
    if (stars)
        stars->render(*renderedImage);
}

/* ------------------------------------------------------------------------*/
