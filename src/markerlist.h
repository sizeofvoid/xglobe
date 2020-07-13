/*
 * markerlist.h
 *
 * $Id: markerlist.h,v 1.4 2000/06/17 13:03:45 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: markerlist.h,v $
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
 * Revision 0.3  1999/07/13 17:31:34  thorsten
 * code beautification
 *
 * Revision 0.2  1999/02/12 17:18:57  thorsten
 * a color is now associated with each location marker
 *
 * Revision 0.1  1998/12/09 18:55:27  thorsten
 * initial revision
 *
 */
#pragma once

#include "compute.h"
#include "random.h"

#include <QImage>
#include <QColor>
#include <QPixmap>
#include <QFont>
#include <QFontMetrics>
#include <QList>
#include <QListIterator>
#include <QRect>
#include <QString>

#include <memory>


class Location {
    friend class MarkerList;

public:
    Location(double lon, double lat, const QString& name, const QColor& color);
    ~Location() {}
    QString getName() const;
    void getLoc(double&, double&, double&) const;
    const QColor& getColor() const;
    QRect boundingRect(const QFontMetrics& fm);
    friend int compare(const Location&, const Location&);
    QRect br;

private:
    double s_x = 0;
    double s_y = 0;
    double s_z = 0;
    double cos_angle = 0;
    int x = 0;
    int y = 0;
    int offset_x = 0;
    int offset_y = 0;
    QString name;
    QColor color;
    Gen gen;

    const int default_offset_x = 4;
    const int default_offset_y = 0;
    const int min_arrow = 5;
};

inline int compare(const Location& l1, const Location& l2)
{
    if (l1.cos_angle > l2.cos_angle)
        return 1;
    if (l1.cos_angle < l2.cos_angle)
        return -1;
    return 0;
}

class MarkerList {
public:
    MarkerList();
    ~MarkerList();
    void append(Location* l);
    void setShift(int x, int y);
    int getShiftX();
    int getShiftY();
    void set_font(const QString& name, int sz);
    void render(const RotMatrix&, QImage&, double, double, double, int, int);

protected:
    QList<Location*> list;
    QListIterator<Location*> list_it;
    void paintMarker(QImage& img, Location* l);
    void paintDot(QImage& img, Location* l);
    void paintArrow(QImage& img, Location* l);

private:
    void render_monochrome(QRgb, QImage&, QImage&, int x, int y);
    void solve_conflicts(Location* visible_locations[], int num);
    QPixmap* markerpixmap = nullptr;
    QImage markerimage;
    QFont* renderFont = nullptr;
    QFontMetrics* fm = nullptr;
    Gen gen;
};

using TMarkerListPtr = std::shared_ptr<MarkerList>;

bool appendMarkerFile(TMarkerListPtr const&, const QString& filename);
