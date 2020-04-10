/*
 * renderer.h
 *
 * $Id: renderer.h,v 1.2 2000/06/15 09:43:30 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: renderer.h,v $
 * Revision 1.2  2000/06/15 09:43:30  espie
 * Bump to what I have
 *
 * Revision 0.7  1999/12/06 16:13:36  thorsten
 * -added -rot switch
 *
 * Revision 0.6  1999/12/05 19:18:53  thorsten
 * by Andrew:
 * - added cloud layer rendering
 *
 * Revision 0.5  1999/09/19 19:46:16  thorsten
 * by Andrew Sumner:
 * - added -shade_area switch
 *
 * Revision 0.4  1999/07/19 12:43:37  thorsten
 * added -term switch
 * added -ambientrgb switch
 *
 * Revision 0.3  1999/07/13 17:42:25  thorsten
 * 8 bit images can now be used without increasing colordepth to 32 bit
 * can now draw stars in the background
 * by Andrew Sumner:
 * can now display a background image
 * globe does no longer need to be centered
 *
 * Revision 0.2  1999/01/06 13:22:34  thorsten
 * added optional grid
 *
 * Revision 0.1  1998/12/09 18:10:25  thorsten
 * initial revision
 *
 */
#pragma once

#include "file.h"
#include "markerlist.h"
#include "random.h"
#include "stars.h"

#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QSize>
#include <QString>
#include <ctime>

static inline QRgb *scan32(QImage &img, int x, int y)
{
	assert(img.depth() == 32);
	return reinterpret_cast<QRgb *>(img.scanLine(y)) + x;
}

class Renderer {
public:
    Renderer(const QSize& size, const QString& mapfile = QString());
    ~Renderer();
    int loadNightMap(const QString& nmapfile = nullptr);
    int loadCloudMap(const QString& cmapfile = nullptr, int cloud_filter = 110);
    int loadBackImage(const QString& imagefile = nullptr, bool tld = false);
    void renderFrame();
    void setViewPos(double lat, double lon);
    double getViewLat();
    double getViewLong();
    double getSunLat();
    double getSunLong();
    void setRotation(double r);
    double getRotation();
    void copyBackImage();
    void setZoom(double z);
    double getZoom();
    void setTime(time_t t);
    time_t getTime();
    void setMarkerList(MarkerList* l);
    void showMarkers(bool show);
    void showLabel(bool show);
    void setShadeArea(double area);
    void setStars(double f, bool show);
    void setLabelPos(int x, int y);
    void setAmbientRGB(QRgba64 const&);
    void setFov(double fov);
    void setNumGridLines(int num);
    int getNumGridLines();
    void setNumGridDots(int num);
    int getNumGridDots();
    void setGridType(int type);
    int getGridType();
    double getStarFrequency();
    QImage* getImage();
    void setShift(int x, int y);
    int getShiftX();
    int getShiftY();
    void setTransition(double t);
    double getTransition();

protected:
    QImage* loadImage(const QString& name);

private:
    void getMapColorLinear(QImage* m, double longitude, double latitude,
        int* r, int* g, int* b);
    unsigned int getPixelColor(double longitude, double latitude,
        double angle);
    void calcLightVector();
    void calcDistance();
    void drawMarkers();
    void drawGrid();
    void drawStars();
    void paintMarker(int x, int y, Location* l);
    void drawLabel();
    static int compareLocations(const void* l1, const void* l2);

public:
    static const int NO_GRID = 0;
    static const int DULL_GRID = 1;
    static const int NICE_GRID = 2;

protected:
    QImage* map;
    QImage* mapnight;
    QImage* mapcloud;
    QImage* backImage;
    QImage* renderedImage;
    MarkerList* markerlist;
    bool show_label;
    int label_x;
    int label_y;
    int shift_x;
    int shift_y;

private:
    bool tiled;
    bool clouds_ok;
    FileChange* track_clouds;

    // stuff used for rendering
    double view_lat;
    double view_long;
    double rot;
    double sun_lat;
    double sun_long;
    time_t time_to_render;
    double radius;
    double proj_dist; // distance to projection plane
    double center_dist; // distance to center of earth
    double ambientRed, ambientGreen, ambientBlue;
    double shade_area;
    double light_x, // vector of sunlight with length 1
        light_y,
        light_z;
    double fov; // field of view
    double zoom;
    int radius_proj; // radius of sphere on screen
    int gridtype;
    double d_gridline; // dist. of grid lines in radians
    double d_griddot; // dist. of grid dots in radians
    double trans; // specifies the smoothness of the transition
        // from day to night
    Gen gen;
    Stars* stars;
    unsigned char v[256]; // values for cloud
};
