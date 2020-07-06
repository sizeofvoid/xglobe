/*
 * earthapp.h
 *
 * $Id: earthapp.h,v 1.3 2000/06/17 12:02:17 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public Licenses as published by
 * the Free Software Foundation.
 *
 * $Log: earthapp.h,v $
 * Revision 1.3  2000/06/17 12:02:17  espie
 * Let marker font be facultative, allow user to change it.
 *
 * Revision 1.2  2000/06/15 09:43:29  espie
 * Bump to what I have
 *
 * Revision 0.8  1999/12/06 16:14:13  thorsten
 * -added -rot switch
 *
 * Revision 0.7  1999/12/05 19:19:27  thorsten
 * - added -outfile switch
 * by Andrew:
 * - added -cloudmapfile switch
 * - added -cloudfilter switch
 *
 * Revision 0.6  1999/09/19 19:45:25  thorsten
 * by Andrew Sumner:
 * - added -shade_area switch
 *
 * Revision 0.5  1999/07/19 12:50:52  thorsten
 * added -term switch
 * by Andrew Sumner:
 * added -ambientrgb switch
 * added moonpos position specifier
 *
 * Revision 0.4  1999/07/13 17:37:11  thorsten
 * added -stars/-starfreq switches
 * by Andrew Sumner:
 * added -backg/-tiled switches
 * added -dumpcmd switch
 * added orbit pos. specifier
 * added -shift switch
 *
 * Revision 0.2  1999/01/06 13:29:56  thorsten
 * added -once option
 * added -timewarp option
 * added -size option
 * improves command line compatibility to xearth
 *
 * Revision 0.1  1998/12/10 20:08:01  thorsten
 * initial revision
 *
 */
#pragma once

#include <QApplication>

#include "markerlist.h"


#include <memory>

class Renderer;
class DesktopWidget;
class QTimer;
class QSize;
class QString;
class CommandLineParser;

class EarthApplication : public QApplication {
    Q_OBJECT

public:
    EarthApplication(int &argc, char **argv);
    ~EarthApplication();

    void init();

private:

    void firstRecalc(time_t);
    void processImage();
    bool adjustMarker();

public slots:
    void recalc();

protected:

    bool builtin_markers = true;
    bool show_markers = true;
    double ambient_red = 0.15;
    double ambient_blue = 0.15;
    double ambient_green = 0.15;
    double fov = 1.;
    bool with_nightmap = false;
    bool with_cloudmap = false;
    bool show_stars = true;
    double star_freq = 0.002;
    char* dumpcmd = nullptr;
    char* dumpfile = nullptr;
    int argc_map = -1;
    int argc_nightmap = -1;
    int argc_cloudmap = -1;
    int argc_bg = 0;
    int cloud_filter = 120;
    time_t start_time;
    time_t current_time;
    double transition = 0.0;
    double shade_area = 1.0;

private:
    std::unique_ptr<CommandLineParser> clp;
    TMarkerListPtr marker_list;
    std::unique_ptr<Renderer> r;
    std::unique_ptr<DesktopWidget> dwidget;
    QTimer* timer = nullptr;
    QString out_file_name;

    bool firstTime = true;
    bool do_dumpcmd = false;

#if defined (XWALLPAPER_BIN)
    const QString xwallpaper_bin = QLatin1String(XWALLPAPER_BIN);
#else
    const QString xwallpaper_bin = QLatin1String("xxx");
#endif

#if defined (DEFAULT_MARKER_FILE)
    const QString default_marker_file = QLatin1String(DEFAULT_MARKER_FILE);
#else
    const QString default_marker_file = QLatin1String("xglobe-markers");
#endif
};
