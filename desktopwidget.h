/*
 * desktopwidget.h
 *
 * $Id: desktopwidget.h,v 1.2 2000/06/15 09:43:29 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: desktopwidget.h,v $
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.1  1998/12/09 18:16:31  thorsten
 * initial revision
 *
 */
#pragma once

#include <QPainter>
#include <QPixmap>
#include <QWidget>

class DesktopWidget : public QWidget {
public:
    DesktopWidget(QWidget* parent = 0, const char* name = 0);
    ~DesktopWidget();
    void paintEvent(QPaintEvent* pe);
    void updateDisplay(QImage* image);

private:
    QPixmap* currentImage;
    bool haveImage;
};
