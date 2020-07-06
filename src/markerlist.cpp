/*
 * markerlist.cpp
 *
 * $Id: markerlist.cpp,v 1.4 2000/06/17 13:03:45 espie Exp $
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
 * $Log: markerlist.cpp,v $
 * Revision 1.4  2000/06/17 13:03:45  espie
 * Move MIN/MAX to compute.h
 * Move solve_conflicts to its own function
 * Compute interpolated pixel more efficiently.
 *
 * Revision 1.3  2000/06/17 12:02:17  espie
 * Let marker font be facultative, allow user to change it.
 *
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.4  1999/07/13 17:32:01  thorsten
 * code beautification
 *
 * Revision 0.3  1999/02/12 17:21:21  thorsten
 * added support for colored markers
 *
 * Revision 0.2  1999/01/06 13:26:48  thorsten
 * minimal change to suppress warning
 *
 * Revision 0.1  1998/12/10 20:00:17  thorsten
 * initial revision
 *
 */

#include "markerlist.h"
#include "compute.h"
#include "renderer.h"
#include "file.h"
#include "marker.xpm"
#include <cstdlib>
#include <math.h>
#include <QFile>
#include <QImage>
#include <QRect>
#include <QPainter>
#include <QTextStream>

const int default_offset_x = 4, default_offset_y = 0, min_arrow = 5;

QRect Location::boundingRect(const QFontMetrics& fm)
{
    QRect br = fm.boundingRect(getName());
#if QT_VERSION >= 230
    static enum { XFT,
        Unknown,
        None } mode
        = Unknown;
    if (mode == Unknown) {
        char* v = ::getenv("QT_XFT");
        if (v != nullptr && strcmp(v, "true") == 0)
            mode = XFT;
        else
            mode = None;
    }
    if (mode == XFT)
        // Kludge dimensions for XRender bug
        br.setRect(br.x(), br.y() - br.height(), br.width(), 2 * br.height());
#endif
    return br;
}

/* ------------------------------------------------------------------------ */

Location::Location(double lon, double lat, const QString& name,
    const QColor& color)
{
    lon *= M_PI / 180.0;
    lat *= M_PI / 180.0;

    const double cs_lat = cos(lat);

    s_x = cs_lat * sin(lon);
    s_y = sin(lat);
    s_z = cos(lat) * cos(lon);
    this->name = name;
    this->color = color;
    offset_x = default_offset_x;
    offset_y = default_offset_y;
}

/* ------------------------------------------------------------------------ */

QString Location::getName() const
{
    return name;
}

/* ------------------------------------------------------------------------ */

void Location::getLoc(double& sx, double& sy, double& sz) const
{
    sx = s_x;
    sy = s_y;
    sz = s_z;
}

/* ------------------------------------------------------------------------ */

const QColor& Location::getColor() const
{
    return color;
}

/* ------------------------------------------------------------------------ */

static bool parse_markerline(QString& line, const QString& filename,
    int linenum, double& lon, double& lat, QString& name, QColor& color)
{
    int pos1, pos2;
    // read longitude
    pos1 = line.indexOf(' ');
    if (pos1 == -1) {
        fprintf(stderr, "Syntax error in marker file \"%s\", line %d.\n",
            filename.toLatin1().data(), linenum);
        return false;
    }
    lat = line.left(pos1).toDouble();

    // read latitude
    pos2 = line.indexOf(' ', pos1 + 1);
    if (pos2 == -1) {
        fprintf(stderr, "Syntax error in marker file \"%s\", line %d.\n",
            filename.toLatin1().data(), linenum);
        return false;
    }
    lon = line.mid(pos1 + 1, pos2 - pos1 - 1).toDouble();

    // read name
    pos1 = line.indexOf('\"', pos2);
    if (pos1 == -1) {
        fprintf(stderr, "Syntax error in marker file \"%s\", line %d.\n",
            filename.toLatin1().data(), linenum);
        return false;
    }
    pos2 = line.indexOf('\"', pos1 + 1);
    if (pos2 == -1) {
        fprintf(stderr, "Syntax error in marker file \"%s\", line %d.\n",
            filename.toLatin1().data(), linenum);
        return false;
    }
    name = line.mid(pos1 + 1, pos2 - pos1 - 1);

    // read color value

    pos1 = line.indexOf("color=", pos2);
    if (pos1 != -1) {
        pos2 = line.indexOf('#', 0);
        if ((pos2 == -1) || (pos2 > pos1)) {
            QString colorname;
            pos2 = line.indexOf(' ', pos1);
            if (pos2 != -1)
                colorname = line.mid(pos1 + 6, pos2 - pos1 - 6);
            else
                colorname = line.right(line.length() - pos1 - 6);
            color.setNamedColor(colorname);
        }
    }
    return true;
}

/*
 * Loads a marker definition file.
 * This file must have the following format:
 *
 * <longitude> <latitude> <name of location>
 * where <longitude> and <latitude> are numbers specifying the geographical
 * position and <name of location> is the place's name in quotes,
 * e.g. "Atlantis".
 * Anything after these three tokens on the same line is ignored.
 * A line with a leading '#' is ignored an can be used for comments.
 */

bool appendMarkerFile(TMarkerListPtr const& l, const QString& filename)
{
    QFile f(FileChange::findXglobeFile(filename));
    QTextStream t(&f);

    if (!f.open(QIODevice::ReadOnly))
        return false;

    int linenum = 0;

    while (!t.atEnd()) {
        QString line = t.readLine();
        linenum++;

        line = line.simplified();
        if (line.isEmpty()) // skip empty lines
            continue;
        if (line.startsWith('#')) // if it's a comment, skip to next line
            continue;

        QColor color = Qt::red;
        double lon, lat;
        QString name;
        if (parse_markerline(line, filename, linenum, lon, lat, name, color))
            l->append(new Location(lon, lat, name, color));
        else {
            f.close();
            return false;
        }
    }
    f.close();
    return true;
}

MarkerList::MarkerList() 
    :  list()
    , list_it(list)
{
    qDeleteAll(list);
    markerpixmap = new QPixmap(marker_xpm);
}

void MarkerList::set_font(const QString& name, int sz)
{
    if (renderFont) {
        delete renderFont;
        renderFont = nullptr;
    }
    if (fm) {
        delete fm;
        fm = nullptr;
    }
    if (name.isEmpty())
        return;

    renderFont = new QFont(name, sz, QFont::Bold);
    if (renderFont == nullptr)
        renderFont = new QFont("helvetica", 12, QFont::Bold);
    fm = new QFontMetrics(*renderFont);
}

MarkerList::~MarkerList()
{
    list.clear();
    delete markerpixmap;
    delete fm;
    delete renderFont;
}

void MarkerList::append(Location* l)
{
    list.append(l);
}

int compareLocations(const void* l1, const void* l2)
{
    const Location** a1 = (const Location**)l1;
    const Location** a2 = (const Location**)l2;
    return compare(**a1, **a2);
}

void MarkerList::solve_conflicts(Location* visible_locations[], int num)
{
    for (int i = 0; i < num; i++) {
        Location* l = visible_locations[i];
        double jitter = 20;
    retry:
        l->br = l->boundingRect(*fm);
        l->br.moveTopLeft(QPoint(l->offset_x + l->x, l->offset_y + l->y));
        for (int j = 0; j < i; j++) {
            QRect in = l->br.intersected(visible_locations[j]->br);
            if (!in.isEmpty()) {
                l->offset_x = gen.gaussian() * jitter;
                l->offset_y = gen.gaussian() * jitter;
                jitter *= 1.05;
                goto retry;
            }
        }
    }
    // second pass: try to remove offsets.
    for (int i = 0; i < num; i++) {
        Location* l = visible_locations[i];
        QRect check = l->boundingRect(*fm);
        check.moveTopLeft(QPoint(default_offset_x + l->x,
            default_offset_y + l->y));
        for (int j = 0; j <= num; j++) {
            if (i == j)
                continue;
            if (j == num) {
                l->br = check;
                l->offset_x = default_offset_x;
                l->offset_y = default_offset_y;
                break;
            }
            QRect in = check.intersected(visible_locations[j]->br);
            if (!in.isEmpty())
                break;
        }
    }
}

void MarkerList::render(const RotMatrix& mat, QImage& dest,
    double radius, double center_dist, double proj_dist,
    int shift_x, int shift_y)
{
    if (list.empty())
        return;

    Location* l;
    double s_x, s_y, s_z;
    double loc_x, loc_y, loc_z;
    int screen_x, screen_y;
    double visible_angle;

    int i, num;
    Location** visible_locations;

    visible_locations = new Location*[count()];
    assert(visible_locations != nullptr);

    visible_angle = radius / center_dist;

    for (i = 0, l = first(); l != nullptr; l = next()) {
        l->getLoc(s_x, s_y, s_z);

        mat.transform(s_x, s_y, s_z, loc_x, loc_y, loc_z);

        l->cos_angle = loc_z / radius;

        if (l->cos_angle < visible_angle)
            // location lies on the other side
            continue;

        loc_y = -loc_y;
        loc_z = center_dist - loc_z;
        screen_x = (int)(loc_x * proj_dist / loc_z);
        screen_y = (int)(loc_y * proj_dist / loc_z);
        screen_x += dest.width() / 2;
        screen_y += dest.height() / 2;

        if ((screen_x < 0) || (screen_x >= dest.width()))
            // location out of bounds
            continue;
        if ((screen_y < 0) || (screen_y >= dest.height()))
            continue;

        l->x = screen_x + shift_x;
        l->y = screen_y + shift_y;

        visible_locations[i] = l;
        i++;
    }

    num = i;

    // sort the markers according to depth
    std::qsort(visible_locations, num, sizeof(Location*), compareLocations);

    if (fm != nullptr)
        solve_conflicts(visible_locations, num);

    for (i = 0; i < num; i++)
        paintDot(dest, visible_locations[i]);

    if (fm != nullptr) {
        for (i = 0; i < num; i++)
            paintArrow(dest, visible_locations[i]);
        for (i = 0; i < num; i++)
            paintMarker(dest, visible_locations[i]);
    }

    delete[] visible_locations;
}

void MarkerList::render_monochrome(QRgb color,
    QImage& img, QImage& l, int x, int y)
{
    QRgb *src, *dest;

    if (l.depth() != 32)
        l = l.convertToFormat(QImage::Format_RGB32);

    QRect screenrect(0, 0, img.width(), img.height());
    QRect labelrect(x, y, l.width(), l.height());
    QRect visiblerect = screenrect.intersected(labelrect);

    if (visiblerect.isEmpty())
        // the label is not visible
        return;

    for (int wy = 0; wy < visiblerect.height(); wy++) {
        dest = scan32(img, visiblerect.x(), visiblerect.y() + wy);
        src = scan32(l, visiblerect.x() - x, visiblerect.y() - y + wy);

        for (int wx = 0; wx < visiblerect.width(); wx++) {
            switch (*src & RGB_MASK) {
            case 0x00000000:
                break;
            case 0x00FF0000:
            case 0x000000FF:
                *dest = 0;
                break;
            case 0x00FFFFFF:
                *dest = color;
                break;
            default:
                // Blue component tells me how to merge
                // Red component tells me what to merge with
                int p = qBlue(*src);
                int v = qRed(*src);
                if (p == 255)
                    *dest = qRgb(qRed(color) * v / 256, qGreen(color) * v / 256,
                        qBlue(color) * v / 256);
                else
                    *dest = qRgb(
                        (p * qRed(color) * v / 256 + (255 - p) * qRed(*dest)) / 256,
                        (p * qGreen(color) * v / 256 + (255 - p) * qGreen(*dest)) / 256,
                        (p * qBlue(color) * v / 256 + (255 - p) * qBlue(*dest)) / 256);
                break;
            }
            dest++;
            src++;
        }
    }
}

void MarkerList::paintMarker(QImage& img, Location* l)
{
    QPainter p;
    int wx, wy;

    QRect br = l->boundingRect(*fm);
    QPixmap pm(6 + br.width(), 4 + br.height());

    p.begin(&pm);
    p.setFont(*renderFont);
    p.fillRect(0, 0, pm.width(), pm.height(), Qt::black);

    // draw a blue seam around the text
    p.setPen(Qt::blue);
    wx = -br.x() + 1;
    wy = -br.y();
    p.drawText(wx, wy + 1, l->getName());
    p.drawText(wx + 1, wy, l->getName());
    p.drawText(wx + 1, wy + 2, l->getName());
    p.drawText(wx + 2, wy + 1, l->getName());

    p.setPen(Qt::white);
    p.drawText(wx + 1, wy + 1, l->getName());
    p.end();

    QImage labelimage = pm.toImage();

    render_monochrome(l->getColor().rgb(), img, labelimage,
        l->x - markerimage.width() / 2 + l->offset_x,
        l->y - markerimage.height() / 2 - pm.height() / 2 + l->offset_y);
}

void MarkerList::paintDot(QImage& img, Location* l)
{
    QPainter p;

    QPixmap pm(markerpixmap->width(), markerpixmap->height());
    p.begin(&pm);
    p.fillRect(0, 0, pm.width(), pm.height(), Qt::black);
    p.setPen(Qt::white);
    p.drawPixmap(0, 0, *markerpixmap);
    p.end();

    QImage markerimage = pm.toImage();
    render_monochrome(l->getColor().rgb(),
        img, markerimage,
        l->x - markerpixmap->width() / 2,
        l->y - markerpixmap->height() / 2);
}

void MarkerList::paintArrow(QImage& img, Location* l)
{
    // Don't paint very short arrows
    if (l->offset_x < min_arrow && l->offset_x > -min_arrow && l->offset_y < min_arrow && l->offset_y > -min_arrow)
        return;
    QPainter p;

    int wx, wy, dx, dy, x1, x2, y1, y2;
    if (l->offset_x >= 0) {
        wx = l->offset_x;
        dx = 0;
        x1 = 0;
        x2 = wx;
        wx++;
    }
    else {
        wx = -l->offset_x;
        dx = l->offset_x;
        x1 = wx;
        x2 = 0;
    }
    if (l->offset_y >= 0) {
        wy = l->offset_y;
        dy = 0;
        y1 = 0;
        y2 = wy;
        wy++;
    }
    else {
        wy = -l->offset_y;
        dy = l->offset_y;
        y1 = wy;
        y2 = 0;
    }
    QPixmap pm(wx, wy);
    p.begin(&pm);
    p.fillRect(0, 0, wx, wy, Qt::black);
    p.setPen(Qt::white);
    p.drawLine(x1, y1, x2, y2);
    p.end();

    QImage markerimage = pm.toImage();
    render_monochrome(l->getColor().rgb(),
        img, markerimage, l->x + dx, l->y + dy);
}
