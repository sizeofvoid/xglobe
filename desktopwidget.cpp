/*
 * desktopwidget.cpp
 *
 * $Id: desktopwidget.cpp,v 1.2 2000/06/15 09:43:29 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: desktopwidget.cpp,v $
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.1  1998/12/09 18:13:56  thorsten
 * initial revision
 *
 */

#include "desktopwidget.h"
#include <stdio.h>
#include <QPaintEvent>
#include <QPalette>
#include <QX11Info>
#include <QBrush>

#include <X11/Xlib.h>



DesktopWidget::DesktopWidget(QWidget* parent, const QString& name)
    : QWidget(parent, Qt::Desktop)
{
    haveImage = false;
    currentImage = new QPixmap(width(), height());
    fprintf(stderr, "Desktop size: %dx%d\n",width(), height());
}

DesktopWidget::~DesktopWidget()
{
    if (currentImage)
        delete currentImage;
}

void DesktopWidget::paintEvent(QPaintEvent* pe)
{
    QPainter p(this);
    fprintf(stderr, "Desktop size: %dx%d\n",width(), height());

    if (!haveImage) {
        p.setFont(QFont("helvetica", 35));
        QRect br = p.fontMetrics().boundingRect("Please wait...");
        p.setPen(QColor(255, 0, 0));
        p.fillRect(0, 0, width(), height(), QColor(0, 0, 0));
        p.drawText((width() - br.width()) / 2, (height() - br.height()) / 2,
            "Please wait...");
    }
    else {
        p.drawPixmap(pe->rect().left(), pe->rect().top(),
            *currentImage, pe->rect().left(), pe->rect().top(),
            pe->rect().width(), pe->rect().height());
    }
}

void DesktopWidget::updateDisplay(QImage* image)
{
    assert(image != nullptr);
    currentImage->convertFromImage(*image);
    haveImage = true;
    QPalette palette;
    palette.setBrush((this)->backgroundRole(),  QBrush(*currentImage));
    (this)->setPalette(palette);
    XClearWindow(QX11Info::display(), QX11Info::appRootWindow());
    update();
}
