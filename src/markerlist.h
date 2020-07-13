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
    ~Location() = default;
    QString getName() const;
    void getLoc(double&, double&, double&) const;
    const QColor& getColor() const;
    QRect boundingRect(const QFontMetrics& fm);
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
using TLocation = std::shared_ptr<Location>;


class MarkerList {
public:
    MarkerList();
    ~MarkerList() = default;
    void append(const TLocation&);
    void setShift(int x, int y);
    int getShiftX();
    int getShiftY();
    void set_font(const QString& name, int sz);
    void render(const RotMatrix&, QImage&, double, double, double, int, int);
    bool appendMarkerFile(const QString&);

protected:
    std::vector<TLocation> locations;
    void paintMarker(QImage& img, const TLocation&);
    void paintDot(QImage& img, const TLocation&);
    void paintArrow(QImage& img, const TLocation&);

private:
    bool parse_markerline(QString&, const QString&, int, double&, double&, QString&, QColor&);
    void render_monochrome(QRgb, QImage&, QImage&, int x, int y);
    void solve_conflicts(std::vector<TLocation>&, int num);
    QImage markerimage;
    std::unique_ptr<QPixmap> markerpixmap;
    std::unique_ptr<QFont> renderFont;
    std::unique_ptr<QFontMetrics> fm;
    Gen gen;
};

using TMarkerListPtr = std::shared_ptr<MarkerList>;
