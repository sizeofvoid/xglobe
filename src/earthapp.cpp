/*
 * earthapp.cpp
 *
 * $Id: earthapp.cpp,v 1.3 2000/06/17 12:02:17 espie Exp $
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
 * $Log: earthapp.cpp,v $
 * Revision 1.3  2000/06/17 12:02:17  espie
 * Let marker font be facultative, allow user to change it.
 *
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.10  1999/12/06 16:13:54  thorsten
 * -added -rot switch
 *
 * Revision 0.9  1999/12/05 19:20:11  thorsten
 * - added -outfile switch
 * by Andrew:
 * - added -cloudmapfile switch
 * - added -cloudfilter switch
 *
 * Revision 0.8  1999/09/19 19:44:44  thorsten
 * by Andrew Sumner:
 * - added -shade_area switch
 * - moonpos bugfix
 *
 * Revision 0.7  1999/08/06 16:15:26  thorsten
 * Command line parsing now works again correctly.
 *
 * Revision 0.6  1999/07/19 12:52:36  thorsten
 * added -term switch
 * by Andrew Sumner:
 * added -ambientrgb switch
 * added moonpos position specifier
 *
 * Revision 0.5  1999/07/13 17:35:23  thorsten
 * added -stars/-starfreq switches
 * by Andrew Sumner:
 * added -backg/-tiles switches
 * added -dumpcmd switch
 * added orbit pos. specifier
 * added -shift switch
 *
 * Revision 0.3  1999/01/10 19:22:27  thorsten
 * forgot to change the default magnification to 1.0. fixed.
 *
 * Revision 0.2  1999/01/08 18:51:52  thorsten
 * added -size switch
 * added -timewarp switch
 * added -grid switches
 * added -once switch
 * improved xearth command-line compatibility
 *
 * Revision 0.1  1998/12/10 20:07:16  thorsten
 * initial revision
 *
 */

#include "earthapp.h"
#include "desktopwidget.h"
#include "renderer.h"
#include "file.h"
#include "moonpos.h"
#include "command_line_parser.h"
#include "geo_coordinate.h"

#include <QTimer>
#include <QString>
#include <QDesktopWidget>
#include <QDebug>
#include <QProcess>

#include <cmath>

#include <unistd.h>
#include <sys/resource.h>
#include <sys/time.h>

EarthApplication::EarthApplication(int &argc, char **argv)
    : QApplication(argc, argv),
      clp(new CommandLineParser(this)),
      out_file_name(clp->getOutputFileName().isEmpty()
                    ? QString("xglobe-dump.png")
                    : clp->getOutputFileName())
{
    auto optNice =clp->getNice();
    if (optNice)
        setpriority(PRIO_PROCESS, getpid(), *optNice);

    if (clp->isKde()) {
        dwidget = std::make_unique<DesktopWidget>();
        dwidget->update();
    }
}

EarthApplication::~EarthApplication(void)
{
    timer->stop();
}

void EarthApplication::init()
{
    const QSize size = clp->getSize();

    if (size.isValid()) {
        r = std::make_unique<Renderer>(size, clp->getMapFileName());
    }
    else {
        r = std::make_unique<Renderer>(clp->isKde() ? dwidget->baseSize() : desktop()->size(),
                                       clp->getMapFileName());
    }

    /* initialize the Renderer */
    const QString nightmapfile = clp->getNightMapfile();
    if (clp->isNightmap() && !nightmapfile.isEmpty())
        r->loadNightMap(nightmapfile);
    else if (!clp->getMapFileName().isEmpty())
        r->loadNightMap(clp->getMapFileName());

    const QString cloudmapfile= clp->getCloudMapFile();
    if (!cloudmapfile.isEmpty())
        r->loadCloudMap(cloudmapfile, clp->getCloudMapFilter());
    else if (!clp->getMapFileName().isEmpty())
        r->loadCloudMap(clp->getMapFileName(), clp->getCloudMapFilter());


    if (!clp->getBackGFileName().isEmpty())
        r->loadBackImage(clp->getBackGFileName(), clp->isTiled());
    r->setViewPos(clp->getGeoCoordinate()->getLatitude(), clp->getGeoCoordinate()->getLongitude());
    r->setZoom(clp->getMag());
    r->setAmbientRGB(clp->computeRgb());
    // XXX No docs
    /*
    if (fov <= 0)
        fov = -1.;
    else if (fov >= 90.)
        fov = -1.;

    if (fov != -1.)
        r->setFov(fov);
    */

    if (clp->isBuiltinMarkers()) {
        if (!appendMarkerFile(marker_list, default_marker_file))
            exit(12);
    }
    marker_list.set_font(clp->getMarkerFont(), clp->getMarkerFontSize());
    if (clp->isShowMarker())
        r->setMarkerList(&marker_list);
    const auto lables = clp->computeLabelPosition();
    r->setLabelPos(std::get<0>(lables), std::get<1>(lables));
    r->setShadeArea(clp->getShadeArea());
    r->showLabel(clp->isShowLabel());
    r->setNumGridLines(clp->getGrid1());
    r->setNumGridDots(clp->getGrid2() * clp->getGrid1() * 4);
    r->setGridType(clp->getGridType());
    r->setStars(clp->getStarFreq(), clp->isStars());
    const auto shift = clp->computeLabelPosition();
    r->setShift(std::get<0>(shift), std::get<1>(shift));
    r->setTransition(clp->getTransition());
    r->setRotation(clp->getRotation());

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(recalc()));
    QTimer::singleShot(1, this, SLOT(recalc())); // this will start rendering
    timer->start(clp->getWait() * 1000); // the 1. image immediately
}

void EarthApplication::recalc()
{
    start_time = time(nullptr); // first image with current time

    if (firstTime) {
        firstRecalc(start_time);
    }

    processImage();

    current_time = time(nullptr) + clp->getWait();
    current_time = (time_t)(start_time + (current_time - start_time) * clp->getTimeWrap());
    r->setTime(current_time);
    switch (clp->getGeoCoordinate()->getType()) {
    case PosType::fixed:
        break;

    case PosType::sunrel:
        r->setViewPos(r->getSunLat() + clp->getGeoCoordinate()->getLatitude(), r->getSunLong() + clp->getGeoCoordinate()->getLongitude());
        break;

    case PosType::random:
        clp->computeRandomPosition();
        r->setViewPos(clp->getGeoCoordinate()->getLatitude(), clp->getGeoCoordinate()->getLongitude());
        break;

    case PosType::orbit:
        {
            auto orbit = std::static_pointer_cast<OrbitCoordinate>(clp->getGeoCoordinate());
            assert(orbit);
            orbit->computePosition(start_time);
            r->setViewPos(orbit->getLatitude(), orbit->getLongitude());
        }
        break;

    case PosType::moonpos:
        {
            double moon_lat, moon_long;
            MoonPos::getMoonPos(start_time, &moon_lat, &moon_long);
            r->setViewPos(moon_lat, moon_long);
        }
        break;
    }
    r->renderFrame();
}

void EarthApplication::firstRecalc(time_t start_time)
{
    firstTime = false;
    processEvents();
    r->setTime(start_time);
    switch (clp->getGeoCoordinate()->getType()) {
    case PosType::fixed:
            break;

    case PosType::sunrel:
        r->setViewPos(r->getSunLat() + clp->getGeoCoordinate()->getLatitude(), r->getSunLong() + clp->getGeoCoordinate()->getLongitude());
        break;

    case PosType::moonpos:
        {
            double moon_lat, moon_long;
            MoonPos::getMoonPos(start_time, &moon_lat, &moon_long);
            r->setViewPos(moon_lat, moon_long);
        }
        break;

    case PosType::random:
        clp->computeRandomPosition();
        r->setViewPos(clp->getGeoCoordinate()->getLatitude(), clp->getGeoCoordinate()->getLongitude());
        break;

    case PosType::orbit:
        {
            auto orbit = std::static_pointer_cast<OrbitCoordinate>(clp->getGeoCoordinate());
            assert(orbit);
            orbit->computePosition(start_time);
            r->setViewPos(orbit->getLatitude(), orbit->getLongitude());
        }
        break;
    }
    r->renderFrame();

    if (clp->isDumpToFile()) {
        QImage* i = r->getImage();
        i->save(out_file_name, "PNG");
        exit(0);
    }
}

void EarthApplication::processImage()
{
    if (clp->isKde()) {
        dwidget->updateDisplay(r->getImage());
        processEvents(); // we want the image to be
    } // displayed immediately
    /* NOT yet
    else if (do_dumpcmd) {
        system(dumpcmd);
        if (clp->isOnce()) {
            processEvents();
            exit(0);
        }
    }
    */
    else {
        QImage* i = r->getImage();
        i->save(clp->getImageTmpFileName(), "PNG");

        QStringList arguments;
        arguments << "--zoom" << clp->getImageTmpFileName();

        qInfo() << "QProcess: " << xwallpaper_bin << arguments;

        QProcess runXwallpaper(this);
        runXwallpaper.start(xwallpaper_bin, arguments);

        if (!runXwallpaper.waitForFinished())
            qDebug() << "failed to execute xwallpaper: " << runXwallpaper.errorString();

        if (clp->isOnce()) {
            processEvents();
            exit(0);
        }
    }
}
