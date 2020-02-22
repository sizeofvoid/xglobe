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

#ifndef _EARTHAPP_H
#define _EARTHAPP_H

#include "desktopwidget.h"
#include "markerlist.h"
#include "random.h"
#include "renderer.h"
#include <qapplication.h>
#include <qsize.h>
#include <qstring.h>
#include <qtimer.h>
#include <time.h>

class EarthApplication : public QApplication {
    Q_OBJECT

public:
    EarthApplication(int ac, char** av);
    virtual ~EarthApplication();

    void init();

private:
    void readPosition(int i);
    void readDelay(int i);
    void readZoom(int i);
    void readBG(int i);
    void readPriority(int i);
    void readMarkerFile(int i);
    void readMarkerFont(int i);
    void readMarkerFontSize(int i);
    void readShift(int i);
    void readLabelPos(int i);
    void readAmbientLight(int i);
    void readAmbientRGB(int i);
    void readMapFile(int i);
    void readNightMapFile(int i);
    void readCloudMapFile(int i);
    void readCloudFilter(int i);
    void readDumpCmd(int i);
    void readFov(int i);
    int readGridVal(int i);
    void readSize(int i);
    void readOrbit(int i);
    void readTimeWarp(int i);
    void readStarFreq(int i);
    void readTransition(int i);
    void readShadeArea(int i);
    void readOutFileName(int i);
    void readRotation(int i);
    void printUsage();
    void printHelp();
    void randomPosition();
    void orbitPosition(time_t);
    void setPriority(int pri);

public slots:
    void recalc();

protected:
    enum PosType { FIXED,
        SUNREL,
        MOONPOS,
        RANDOM,
        ORBIT };

    Renderer* r;
    DesktopWidget* dwidget;
    QTimer* timer;
    double view_lat, view_long;
    double orbit_period;
    double orbit_inclin;
    double orbit_shift;
    double zoom;
    int delay;
    PosType p_type;
    bool builtin_markers;
    bool show_markers;
    bool show_label;
    int label_x, label_y;
    int shift_x, shift_y;
    double ambient_red, ambient_blue, ambient_green;
    double fov;
    bool with_nightmap;
    bool with_cloudmap;
    bool with_bg;
    bool tiled;
    bool show_stars;
    double star_freq;
    char* dumpcmd;
    char* dumpfile;
    int argc_map, argc_nightmap, argc_cloudmap, argc_bg;
    int cloud_filter;
    time_t start_time;
    time_t current_time;
    double time_warp;
    MarkerList marker_list;
    const char* markerfont;
    int markerfontsize;
    int grid_type, grid1, grid2;
    double transition;
    double shade_area;
    double rotation;
    QString out_file_name;

private:
    bool firstTime;
    bool do_the_dump;
    bool do_dumpcmd;
    bool use_kde;
    bool once;
    bool have_size;
    QSize size;
    Gen gen;
};

#endif
