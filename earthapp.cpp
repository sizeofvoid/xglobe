/*
 * earthapp.cpp
 *
 * $Id: earthapp.cpp,v 1.3 2000/06/17 12:02:17 espie Exp $
 *
 * This file is part of XGlobe. See README for details.
 *
 * Copyright (C) 1998 Thorsten Scheuermann
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

#include <QSize>
#include <QTimer>
#include <QString>
#include <QDesktopWidget>
#include <QPaintEvent>
#include <QPalette>
#include <QX11Info>
#include <QBrush>
#include <QDebug>
#include <QScreen>

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <X11/Xlib.h>

/* ------------------------------------------------------------------------*/

EarthApplication::EarthApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
    firstTime = true;
    view_lat = 0.;
    view_long = 0.;
    shift_x = shift_y = 0;
    delay = 3;
    zoom = 1.0;
    p_type = SUNREL;
    builtin_markers = true;
    show_markers = true;
    show_label = true;
    label_x = -5;
    label_y = 5;
    ambient_red = ambient_green = ambient_blue = 0.15;
    fov = -1.;
    with_nightmap = with_cloudmap = false;
    cloud_filter = 120;
    with_bg = false;
    tiled = false;
    argc_map = -1;
    argc_nightmap = -1;
    argc_cloudmap = -1;
    do_the_dump = false;
    do_dumpcmd = false;
    dumpcmd = dumpfile = nullptr;
    dwidget = nullptr;
    use_kde = false;
    once = false;
    time_warp = 1.0;
    have_size = false;
    grid_type = Renderer::NO_GRID;
    grid1 = 6;
    grid2 = 15;
    orbit_period = orbit_inclin = orbit_shift = 0;
    show_stars = true;
    star_freq = 0.002;
    transition = 0.0;
    shade_area = 1.0;
    out_file_name = QString("xglobe-dump.bmp");
    rotation = 0.0;
    markerfont = "helvetica";
    markerfontsize = 12;

    // evaluate command line parameters
    /*
    for (QString arg : arguments()) {
        if (strcmp(argv()[i], "-pos") == 0) {
            readPosition(++i);
        }
        else if (strcmp(argv()[i], "-wait") == 0) {
            readDelay(++i);
        }
        else if (strcmp(argv()[i], "-mag") == 0) {
            readZoom(++i);
        }
        else if (strcmp(argv()[i], "-backg") == 0) {
            readBG(++i);
        }
        else if (strcmp(argv()[i], "-dir") == 0) {
            set_userdir(argv()[++i]);
        }
        else if (strcmp(argv()[i], "-tiled") == 0) {
            tiled = true;
        }
        else if (strcmp(argv()[i], "-center") == 0) {
            tiled = false;
        }
        else if (strcmp(argv()[i], "-nice") == 0) {
            readPriority(++i);
        }
        else if (strcmp(argv()[i], "-markers") == 0) {
            builtin_markers = true;
            show_markers = true;
        }
        else if (strcmp(argv()[i], "-nomarkers") == 0) {
            builtin_markers = false;
            show_markers = false;
        }
        else if (strcmp(argv()[i], "-markerfont") == 0) {
            readMarkerFont(++i);
            show_markers = true;
        }
        else if (strcmp(argv()[i], "-markerfontsize") == 0) {
            readMarkerFontSize(++i);
            show_markers = true;
        }
        else if (strcmp(argv()[i], "-markerfile") == 0) {
            readMarkerFile(++i);
            show_markers = true;
        }
        else if (strcmp(argv()[i], "-shift") == 0) {
            readShift(++i);
        }
        else if (strcmp(argv()[i], "-label") == 0) {
            show_label = true;
        }
        else if (strcmp(argv()[i], "-nolabel") == 0) {
            show_label = false;
        }
        else if (strcmp(argv()[i], "-labelpos") == 0) {
            readLabelPos(++i);
        }
        else if (strcmp(argv()[i], "-ambientlight") == 0) {
            readAmbientLight(++i);
        }
        else if (strcmp(argv()[i], "-ambientrgb") == 0) {
            readAmbientRGB(++i);
        }
        else if (strcmp(argv()[i], "-fov") == 0) {
            readFov(++i);
        }
        else if (strcmp(argv()[i], "-nightmap") == 0) {
            with_nightmap = true;
        }
        else if (strcmp(argv()[i], "-nonightmap") == 0) {
            with_nightmap = false;
        }
        else if (strcmp(argv()[i], "-mapfile") == 0 || strcmp(argv()[i], "-map") == 0) {
            readMapFile(++i);
        }
        else if (strcmp(argv()[i], "-nightmapfile") == 0 || strcmp(argv()[i], "-nightmap") == 0 || strcmp(argv()[i], "-night") == 0) {
            readNightMapFile(++i);
            with_nightmap = true;
        }
        else if (strcmp(argv()[i], "-cloudmapfile") == 0 || strcmp(argv()[i], "-cloudmap") == 0 || strcmp(argv()[i], "-cloud") == 0 || strcmp(argv()[i], "-clouds") == 0) {
            readCloudMapFile(++i);
            with_cloudmap = true;
        }
        else if (strcmp(argv()[i], "-cloudfilter") == 0 || strcmp(argv()[i], "-filter") == 0) {
            readCloudFilter(++i);
            with_cloudmap = true;
        }
        else if (strcmp(argv()[i], "-maps") == 0) {
            readMapFile(++i);
            readNightMapFile(++i);
            readCloudMapFile(++i);
            with_nightmap = with_cloudmap = true;
        }
        else if (strcmp(argv()[i], "-help") == 0) {
            printHelp();
            ::exit(0);
        }
        else if (strcmp(argv()[i], "-dump") == 0) {
            do_the_dump = true;
        }
        else if (strcmp(argv()[i], "-outfile") == 0) {
            readOutFileName(++i);
        }
        else if (strcmp(argv()[i], "-dumpcmd") == 0) {
            readDumpCmd(++i);
            do_dumpcmd = true;
        }
        else if (strcmp(argv()[i], "-kde") == 0) {
            use_kde = true;
        }
        else if (strcmp(argv()[i], "-once") == 0) {
            once = true;
        }
        else if (strcmp(argv()[i], "-timewarp") == 0) {
            readTimeWarp(i + 1);
            i++;
        }
        else if (strcmp(argv()[i], "-size") == 0) {
            readSize(++i);
            have_size = true;
        }
        else if (strcmp(argv()[i], "-nogrid") == 0) {
            grid_type = Renderer::NO_GRID;
        }
        else if (strcmp(argv()[i], "-grid") == 0) {
            grid_type = Renderer::DULL_GRID;
        }
        else if (strcmp(argv()[i], "-newgrid") == 0) {
            grid_type = Renderer::NICE_GRID;
        }
        else if (strcmp(argv()[i], "-grid1") == 0) {
            grid1 = readGridVal(++i);
        }
        else if (strcmp(argv()[i], "-grid2") == 0) {
            grid2 = readGridVal(++i);
        }
        else if (strcmp(argv()[i], "-stars") == 0) {
            show_stars = true;
        }
        else if (strcmp(argv()[i], "-nostars") == 0) {
            show_stars = false;
        }
        else if (strcmp(argv()[i], "-starfreq") == 0) {
            readStarFreq(++i);
        }
        else if (strcmp(argv()[i], "-term") == 0) {
            readTransition(++i);
        }
        else if (strcmp(argv()[i], "-shade_area") == 0) {
            readShadeArea(++i);
        }
        else if (strcmp(argv()[i], "-rot") == 0) {
            readRotation(++i);
        }
        else {
            fprintf(stderr, "Unknown command line switch: %s\n\n", argv()[i]);
            printUsage();
            ::exit(1);
        }
    }
*/

    QRect  screenGeometry = primaryScreen()->geometry();
    const int height = screenGeometry.height();
    const int width = screenGeometry.width();

    qInfo() << "Screengeometry height: " << height << " width: " << width;

    if (once || do_the_dump)
        use_kde = false;

    if (use_kde) {
        dwidget = new DesktopWidget();
        dwidget->update();
    }
}

/* ------------------------------------------------------------------------*/

EarthApplication::~EarthApplication(void)
{
    delete r;
    timer->stop();
    delete timer;

    delete dwidget;
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readPosition(int i)
{
    int pos;
    /*

    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }

    QString s(argv()[i]);
    s.simplified();

    if (strncmp(argv()[i], "random", 6) == 0) {
        p_type = RANDOM;
        return;
    }
    else if (strncmp(argv()[i], "fixed", 5) == 0)
        p_type = FIXED;
    else if (strncmp(argv()[i], "sunrel", 6) == 0)
        p_type = SUNREL;
    else if (strncmp(argv()[i], "moonpos", 7) == 0) {
        p_type = MOONPOS;
        return;
    }
    else if (strncmp(argv()[i], "orbit", 5) == 0)
        p_type = ORBIT;
    else {
        printUsage();
        ::exit(1);
    }

    if (p_type == ORBIT) {
        // read period and inclination for orbit mode
        pos = s.find(' ');
        if (pos == -1) {
            printUsage();
            ::exit(1);
        }
        s = s.right(s.length() - pos - 1);

        pos = s.find(' ');
        if (pos == -1) {
            printUsage();
            ::exit(1);
        }

        orbit_period = s.left(pos).toDouble() * 3600;
        orbit_inclin = s.right(s.length() - pos - 1).toDouble();

        pos = s.find(' ');
        if (pos != -1)
            orbit_shift = s.right(s.length() - pos - 1).toDouble();

        if (orbit_period <= 0) {
            fprintf(stderr, "orbit period must be a positive number.\n");
            ::exit(1);
        }
        if (orbit_inclin > 90 || orbit_inclin < -90) {
            fprintf(stderr, "orbit inclination must be between -90 and 90\n");
            ::exit(1);
        }
        if (orbit_shift < 0) {
            fprintf(stderr, "orbit shift must be larger than or equal to zero\n");
            ::exit(1);
        }

        view_lat = view_long = 0;
        return;
    }

    // read longitude and latitude for fixed and sunrel mode
    pos = s.find(' ');
    if (pos == -1)
        pos = s.find(',');
    if (pos == -1)
        pos = s.find('/');
    if (pos == -1) {
        printUsage();
        ::exit(1);
    }
    s = s.right(s.length() - pos - 1);

    pos = s.find(' ');
    if (pos == -1)
        pos = s.find(',');
    if (pos == -1)
        pos = s.find('/');
    if (pos == -1) {
        printUsage();
        ::exit(1);
    }

    view_lat = s.left(pos).toDouble();
    view_long = s.right(s.length() - pos - 1).toDouble();
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readBG(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }

    argc_bg = i;
    with_bg = true;
    show_stars = false;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readDelay(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    delay = atoi(argv()[i]);
    if (delay < 1)
        delay = 300;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readZoom(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    zoom = atof(argv()[i]);
    if (zoom <= 0.)
        zoom = 1.0;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readPriority(int i)
{
    /*
    int pri;

    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    pri = atoi(argv()[i]);
    setPriority(pri);
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readMarkerFile(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    if (!appendMarkerFile(marker_list, argv()[i])) {
        // bail out
        ::exit(1);
    }
    */
}

void EarthApplication::readMarkerFont(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    markerfont = argv()[i];
    if (strcmp(markerfont, "none") == 0)
        markerfont = nullptr;
    */
}

void EarthApplication::readMarkerFontSize(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    markerfontsize = atoi(argv()[i]);
    if (markerfontsize <= 5)
        markerfontsize = 5;
    else if (markerfontsize >= 50)
        markerfontsize = 50;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readLabelPos(int i)
{
    /*
    char* s;
    int n;

    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }

    s = argv()[i];

    switch (*s) {
    case '+':
        label_x = 1;
        break;

    case '-':
        label_x = -1;
        break;

    default:
        printUsage();
        ::exit(1);
    }

    n = atoi(++s);
    if (n != 0) // to preserve the sign when 0 is used as parameter
        label_x *= n;

    while (isdigit(*++s))
        ;

    switch (*s) {
    case '+':
        label_y = 1;
        break;

    case '-':
        label_y = -1;
        break;

    default:
        printUsage();
        ::exit(1);
    }
    n = atoi(++s);
    if (n != 0) // to preserve the sign when 0 is used as parameter
        label_y *= n;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readShift(int i)
{
    /*
    int pos;

    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }

    QString s(argv()[i]);
    s.simplified();

    pos = s.find(' ');
    if (pos == -1) {
        printUsage();
        ::exit(1);
    }

    shift_x = s.left(pos).toInt();
    shift_y = s.right(s.length() - pos - 1).toInt();
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readSize(int i)
{
    /*
    int pos;
    int width, height;

    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }

    QString s(argv()[i]);
    s.simplified();

    pos = s.find(' ');
    if (pos == -1)
        pos = s.find(',');
    if (pos == -1)
        pos = s.find('/');
    if (pos == -1) {
        printUsage();
        ::exit(1);
    }

    width = s.left(pos).toInt();
    height = s.right(s.length() - pos - 1).toInt();
    if ((width <= 0) || (height <= 0)) {
        printUsage();
        ::exit(1);
    }
    size.setWidth(width);
    size.setHeight(height);
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readAmbientLight(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    ambient_red = atof(argv()[i]);
    if (ambient_red > 100.)
        ambient_red = 100.;
    else if (ambient_red < 0.)
        ambient_red = 0.;

    ambient_red /= 100.;
    ambient_green = ambient_blue = ambient_red;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readAmbientRGB(int i)
{
    /*
    int pos;

    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }

    QString s(argv()[i]);
    s.simplified();

    pos = s.find(' ');
    if (pos == -1)
        pos = s.find(',');
    if (pos == -1)
        pos = s.find('/');
    if (pos == -1) {
        printUsage();
        ::exit(1);
    }

    ambient_red = s.left(pos).toDouble();
    ambient_green = s.right(s.length() - pos - 1).toDouble();
    s = s.right(s.length() - pos - 1);
    pos = s.find(' ');
    if (pos == -1)
        pos = s.find(',');
    if (pos == -1)
        pos = s.find('/');
    if (pos == -1) {
        printUsage();
        ::exit(1);
    }
    ambient_blue = s.right(s.length() - pos - 1).toDouble();

    if (ambient_red > 100.)
        ambient_red = 100.;
    else if (ambient_red < 0.)
        ambient_red = 0.;
    ambient_red /= 100.;
    if (ambient_green > 100.)
        ambient_green = 100.;
    else if (ambient_green < 0.)
        ambient_green = 0.;
    ambient_green /= 100.;
    if (ambient_blue > 100.)
        ambient_blue = 100.;
    else if (ambient_blue < 0.)
        ambient_blue = 0.;
    ambient_blue /= 100.;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readFov(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    fov = atof(argv()[i]);
    if (fov <= 0)
        fov = -1.;
    else if (fov >= 90.)
        fov = -1.;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readTimeWarp(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    time_warp = atof(argv()[i]);
    if (time_warp < 0.0) {
        printf("Error: timewarp must be positive!\n");
        printUsage();
        ::exit(1);
    }
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readMapFile(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    argc_map = i;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readDumpCmd(int i)
{
    /*
    char cwd[512];

    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }

    getcwd(cwd, 512);
    dumpcmd = (char*)malloc(strlen(argv()[i]) + strlen(cwd) + 20);
    sprintf(dumpcmd, "%s %s/xglobe.bmp", argv()[i], cwd);
    dumpfile = (char*)malloc(strlen(cwd) + 20);
    sprintf(dumpfile, "%s/xglobe.bmp", cwd);
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readNightMapFile(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    argc_nightmap = i;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readCloudMapFile(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    argc_cloudmap = i;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readCloudFilter(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    cloud_filter = atoi(argv()[i]);
    if (cloud_filter < 0)
        cloud_filter = 0;
    if (cloud_filter > 255)
        cloud_filter = 255;
    */
}

/* ------------------------------------------------------------------------*/

int EarthApplication::readGridVal(int i)
{
    /*
    int n;

    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    n = atoi(argv()[i]);
    if (n <= 0) {
        printUsage();
        ::exit(1);
    }
    return n;
    */
    return 0;
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readStarFreq(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    star_freq = atof(argv()[i]);
    if (star_freq < 0.0) {
        printf("Error: star frequency must be positive!\n");
        printUsage();
        ::exit(1);
    }
    */
}
/* ------------------------------------------------------------------------*/

void EarthApplication::readTransition(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    transition = atof(argv()[i]);
    if ((transition < 0.0) || (transition > 100.0)) {
        printUsage();
        ::exit(1);
    }
    transition /= 100.0;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readShadeArea(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    shade_area = atof(argv()[i]);
    if ((shade_area < 0.0) || (shade_area > 100.0)) {
        printUsage();
        ::exit(1);
    }
    shade_area /= 100.0;
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readRotation(int i)
{
    /*
    if (i >= argc()) {
        printUsage();
        ::exit(1);
    }
    rotation = atof(argv()[i]);
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::readOutFileName(int i)
{
    /*
    out_file_name = QString(argv()[i]);
    */
}

/* ------------------------------------------------------------------------*/

void EarthApplication::randomPosition()
{
    view_lat = (gen(30001) / 30000.) * 180. - 90.;
    view_long = (gen(30001) / 30000.) * 360. - 180.;
}

/* ------------------------------------------------------------------------*/


void EarthApplication::orbitPosition(time_t ssue)
{
    double x, y, z;
    double a, c, s;
    double t1, t2, shift;

    /* start at 0 N 0 E */
    x = 0;
    y = 0;
    z = 1;

    /* rotate in about y axis (from z towards x) according to the number
   * of orbits we've completed
   */
    a = (((double)ssue) / orbit_period) * (2 * M_PI);
    shift = fmod((a * orbit_shift), 360);
    c = cos(a);
    s = sin(a);
    t1 = c * z - s * x;
    t2 = s * z + c * x;
    z = t1;
    x = t2;

    /* rotate about z axis (from x towards y) according to the
   * inclination of the orbit
   */
    a = orbit_inclin * (M_PI / 180);
    c = cos(a);
    s = sin(a);
    t1 = c * x - s * y;
    t2 = s * x + c * y;
    x = t1;
    y = t2;

    /* rotate about y axis (from x towards z) according to the number of
   * rotations the earth has made
   */
    a = ((double)ssue / 86400) * (2 * M_PI);
    c = cos(a);
    s = sin(a);
    t1 = c * x - s * z;
    t2 = s * x + c * z;
    x = t1;
    z = t2;

    view_lat = asin(y) * (180 / M_PI);
    view_long = atan2(x, z) * (180 / M_PI);
    if (view_long + shift > 180)
        view_long = -180 + (shift - (180 - view_long));
    else
        view_long += shift;
}

/* ------------------------------------------------------------------------*/

void EarthApplication::setPriority(int pri)
{
    setpriority(PRIO_PROCESS, getpid(), pri);
}

/* ------------------------------------------------------------------------*/

void EarthApplication::printUsage()
{
    printf("XGlobe %s\n", VERSION);
    printf("Usage: %s [-pos pos_spec] [-wait seconds] [-mag factor] [-rot angle]\n"
           "[-markers|-nomarkers] [-markerfile file] [-label|-nolabel] [-labelpos geom]\n"
           "[-ambientlight level] [-ambientrgb rgblevel] [-nice priority]\n"
           "[-nightmap|-nonightmap] [-mapfile file] [-nightmapfile file] \n"
           "[-cloudmapfile file] [-cloudfilter n] [-dump] [-dumpcmd cmd] [-outfile file]\n"
           "[-once] [-grid|-nogrid|-newgrid] [-grid1 grid1] [-grid2 grid2]\n"
           "[-timewarp factor] [-size size_spec] [-shift spec] [-backg file] [-tiled]\n"
           "[-kde] [-stars|-nostars] [-starfreq frequency] [-term pct] [-shade_area pct]\n"
           "[-help]\n"
           "For an explanation of command line options, use the switch -help.\n",
        "xglobe");
}

/* ------------------------------------------------------------------------*/

void EarthApplication::printHelp()
{
    printf("XGlobe %s command line options:\n\n", VERSION);
    printf("-pos pos_spec  pos_spec consists of one of the keywords \"fixed\", \"sunrel\",\n"
           "               \"random\" or \"orbit\", possibly followed by additional arguments:\n"
           "               fixed: Followed by two numerical arguments latitude and\n"
           "                 longitude (given in decimal degrees) of a viewing position.\n"
           "                 - positive lat. - north of equator\n"
           "                 - negative lat. - south of equator\n"
           "                 - positive long. - east of prime meridian\n"
           "                 - negative long. - west of prime meridian\n"
           "               sunrel: Followed by to numerical arguments lat. and long.,\n"
           "                 indicating offsets of current sun position.\n"
           "               moonpos: Viewing position follows current moon position.\n"
           "               random: selects a random viewing position each time a frame is\n"
           "                 redrawn.\n"
           "               orbit:  The position specifier keyword orbit should be followed\n"
           "                 by three arguments, interpreted as numerical values indicating\n"
           "                 the period (in hours), orbital inclination (in decimal degrees)\n"
           "                 of a simple circular orbit, and an experimental shift modifier\n"
           "                 that adjusts the orbit with each circuit; the viewing position\n"
           "                 follows this orbit.\n"
           "                 Astute  readers will surely note that these parameters are not\n"
           "                 sufficient to uniquely specify a single circular orbit. This\n"
           "                 problem is solved by limiting the space of possible orbits to\n"
           "                 those positioned over 0 degrees latitude, 0 degrees longitude\n"
           "                 at time zero (the Un*x epoch, see time(3)).\n"
           "               Examples explaining characters to delimit specifier components:\n"
           "                  -pos 'fixed 30.0 40.53'\n"
           "                  -pos sunrel,-12.3,130\n"
           "                  -pos fixed/20.0/30.0}\n"
           "                  -pos 'orbit 4.2 26.3'\n"
           "               (default: -pos 'sunrel 0 0')\n\n");
    printf("-wait secs     Specifies the interval in seconds between screen updates.\n"
           "               (default: -wait 300)\n\n");
    printf("-mag factor    Specifies the size of the globe in relation to the screen size.\n"
           "               The diameter of the globe is factor times the shorter of the\n"
           "               width and height of the screen.\n"
           "               (default: -mag 1.0)\n\n");
    printf("-dir dir   Set lookup directory for files.\n\n");
    printf("-rot angle     A positive angle rotates the globe clockwise, a negative one\n"
           "               counterclockwise.\n\n");
    printf("-markers       Enable displaying of built-in display markers.\n"
           "-nomarkers     Disable displaying of built-in display markers.\n"
           "               (default: -marker)\n\n");
    printf("-markerfile file  Load an additional location marker file. (Have a look at\n"
           "                  file \"xglobe-markers\" for reference.)\n\n");
    printf("-label         Enable displaying of label indicating current date and time etc.\n"
           "-nolabel       Disable displaying of label\n"
           "               (default: -label)\n\n");
    printf("-labelpos geom  geom specifies the screen location of the label.\n"
           "                Syntax: {+-}<xoffset>{+-}<yoffset>\n"
           "                   positive xoffset - offset from right display edge\n"
           "                   negative xoffset - offset from left display edge\n"
           "                   positive yoffset - offset from top display edge\n"
           "                   negative yoffset - offset from bottom display edge\n"
           "                (default: -labelpos -5+5)\n\n");
    printf("-ambientlight level  Indicates how the dark side of the globe appears:\n"
           "                     0 means totally black, 100 means totally bright (= no\n"
           "                     difference between day and night side).\n"
           "                     (default: -ambientlight 15)\n\n");
    printf("-ambientrgb rgblevel Works like -ambientlight but takes 3 parameters (red,\n"
           "                     green and blue value) defining the color of ambient\n"
           "                     light. This can be useful in conjunction with a night\n"
           "                     map that is tinted towards blue, for example. Using a\n"
           "                     blueish ambient light makes the transition from day to\n"
           "                     night look better.\n"
           "                     Use either -ambientlevel or -ambientrgb, not both.\n"
           "                     (example: -ambientrgb \"1 4 20\"  - This will make the\n"
           "                     night side appear blueish.)\n\n");
    printf("-nice priority Run the xglobe process with the given priority (see nice(1) and\n"
           "               setpriority(2) manual pages).\n\n");
    printf("-nightmap      Use default world map for the night side. The switch\n"
           "               -ambientlight will be ignored.\n"
           "-nonightmap    Disable use of night map.\n\n");
    printf("-mapfile|-map file  Use another than the default world map. Supported image\n"
           "               formats depend on qt.\n\n");
    printf("-nightmapfile|-nightmap|-night file  Same as -mapfile, but for the night map.\n\n");
    printf("-cloudmapfile|-cloudmap|-clouds file  Same as -mapfile, but for the cloud map.\n\n");
    printf("-maps map night clouds  All three maps in one option.\n\n");
    printf("-cloudfilter|-filter n Used in conjunction with -cloudmapfile, this controls\n"
           "               how much cloud is displayed.  n is a value between 0 and 255,\n"
           "               where 0 will show all cloud, and 255 will only show the\n"
           "               brightest clouds.  The default value is 120.\n");
    printf("-backg file    Use the image in file as the screen background, instead of a\n"
           "               black screen, which is the default.\n\n");
    printf("-tiled         The background image specified using -backg is by default\n"
           "               expanded to fill the screen.  This option will cause it to\n"
           "               be tiled instead.\n\n");
    printf("-kde           Use this option when the globe disappears after switching\n"
           "               virtual screens. This is needed when using KDE.\n\n");
    printf("-dump          Saves the rendered image to the file specified with the -outfile\n"
           "               option (default: \"xglobe-dump.bmp\") instead of displaying it\n"
           "               on screen.\n\n");
    printf("-dumpcmd cmd   Saves the rendered image to \"xglobe.bmp\" in the current\n"
           "               directory, then executes \"cmd\", passing the image filename\n"
           "               as an argument, eg '-dumpcmd Esetroot'.\n\n");
    printf("-outfile file  Specifies the output file name for the -dump option.\n\n");
    printf("-once          With this option, XGlobe renders an image once and exits.\n\n");
    printf("-grid          Enable displaying of grid on the globe.\n"
           "-nogrid        Disable displaying of grid on the globe.\n"
           "-newgrid       Enable displaying of a more fancy grid.\n\n");
    printf("-grid1 grid1   Specify the spacing of major grid lines: they are drawn with a\n"
           "               90/grid1 degree spacing.\n"
           "               (default: -grid1 6 which corresponds to 15(o) between grid lines)\n\n");
    printf("-grid2 grid2   Specify spacing of dots along major grid lines. Along the\n"
           "               equator and lines of longitude, grid dots are drawn with a\n"
           "               90/(grid1 x grid2) degree spacing.\n"
           "               (default: -grid2 15 which corresponds, along with -grid1 6,\n"
           "               to a 1(o) spacing)\n\n");
    printf("-timewarp factor  Scale the apparent rate at which time progresses by 'factor'.\n"
           "                  (default: -timewarp 1.0)\n\n");
    printf("-size size_spec Specify the size of the image to be rendered (useful in\n"
           "                conjuntion with -dump). size_spec consists of two components,\n"
           "                both positive integers. They are interpreted as the width and\n"
           "                height (in pixels) of the image.\n"
           "                The details provided for position specifiers (see above) about\n"
           "                the characters used to delimit specifier components apply to\n"
           "                size specifiers as well.\n"
           "                (default: size of the desktop)\n\n");
    printf("-shift spec    Specify that the center of the rendered image should be shifted\n"
           "               by some amount from the center of the image. The spec consists\n"
           "               of two components, both integers; these components are\n"
           "               interpreted as the offsets (in pixels) in the X and Y\n"
           "               directions.\n"
           "               By default, the center of the rendered image is aligned with the\n"
           "               center of the image.\n\n");
    printf("-stars         Enable displaying of stars in the background (default)\n"
           "-nostars       Disable displaying of stars in the background\n\n");
    printf("-starfreq frequency    If displaying of stars is enabled, frequency percent\n"
           "                       of the background pixels are turned into stars\n"
           "                       (default: 0.002%%)\n\n");
    printf("-term pct      Specify the shading discontinuity at the terminator (day/night\n"
           "               line). Pct should be between 0 and 100, where 100 is maximum\n"
           "               discontinuity and 0 makes a very smooth transition.\n"
           "               (default: -term 0)\n\n");
    printf("-shade_area pct Specify the proportion of the day-side to be progressively\n"
           "                shaded prior to a transition with the night-side.  A value of\n"
           "                100 means all the day area will be shaded, whereas 0 will result\n"
           "                in no shading at all.  60 would keep 40%% of the day area nearest\n"
           "                the sun free from shading. (default: -shade_area 100)\n\n");
    printf("-help          Display this help text.\n\n");
}

/* ------------------------------------------------------------------------*/

void EarthApplication::init()
{
    QString std_marker_filename("xglobe-markers");

    if (have_size) {
        r = new Renderer(size,
                QString());
            //(argc_map != -1) ? argv()[argc_map] : QString());
    }
    else {
        r = new Renderer(use_kde ? dwidget->baseSize() : desktop()->size(),
                QString());
            //(argc_map != -1) ? argv()[argc_map] : QString());
            //(argc_map != -1) ? argv()[argc_map] : QString());
    }

    /* initialize the Renderer */
    if (with_nightmap)
        r->loadNightMap(QString());
        //r->loadNightMap((argc_nightmap != -1) ? argv()[argc_nightmap] : (const char*)nullptr);
    if (with_cloudmap)
        r->loadCloudMap(QString());
        //r->loadCloudMap((argc_cloudmap != -1) ? argv()[argc_cloudmap] : (const char*)nullptr, cloud_filter);
    if (with_bg)
        r->loadBackImage(QString());
        //r->loadBackImage(((argc_bg != -1) ? argv()[argc_bg] : (const char*)nullptr), tiled);
    r->setViewPos(view_lat, view_long);
    r->setZoom(zoom);
    r->setAmbientRGB(ambient_red, ambient_green, ambient_blue);
    if (fov != -1.)
        r->setFov(fov);

    if (builtin_markers) {
        if (!appendMarkerFile(marker_list, std_marker_filename))
            ::exit(12);
    }
    marker_list.set_font(markerfont, markerfontsize);
    if (show_markers)
        r->setMarkerList(&marker_list);
    r->setLabelPos(label_x, label_y);
    r->setShadeArea(shade_area);
    r->showLabel(show_label);
    r->setNumGridLines(grid1);
    r->setNumGridDots(grid2 * grid1 * 4);
    r->setGridType(grid_type);
    r->setStars(star_freq, show_stars);
    r->setShift(shift_x, shift_y);
    r->setTransition(transition);
    r->setRotation(rotation);

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(recalc()));
    QTimer::singleShot(1, this, SLOT(recalc())); // this will start rendering
    timer->start(delay * 1000); // the 1. image immediately
}

/* ------------------------------------------------------------------------*/

void EarthApplication::recalc()
{
    double moon_lat, moon_long;

    if (firstTime) {
        firstTime = false;
        start_time = time(nullptr); // first image with current time
        processEvents();
        r->setTime(start_time);
        switch (p_type) {
        case SUNREL:
            r->setViewPos(r->getSunLat() + view_lat, r->getSunLong() + view_long);
            break;

        case MOONPOS:
            MoonPos::getMoonPos(start_time, &moon_lat, &moon_long);
            r->setViewPos(moon_lat, moon_long);
            break;

        case RANDOM:
            randomPosition();
            r->setViewPos(view_lat, view_long);
            break;

        case ORBIT:
            orbitPosition(start_time);
            r->setViewPos(view_lat, view_long);
            break;

        default:
            break;
        }
        r->renderFrame();

        if (do_the_dump) {
            QImage* i = r->getImage();
            i->save(out_file_name, "BMP");
            ::exit(0);
        }
    }

    if (use_kde) {
        dwidget->updateDisplay(r->getImage());
        processEvents(); // we want the image to be
    } // displayed immediately
    else if (do_dumpcmd) {
        QImage* i = r->getImage();
        i->save(dumpfile, "BMP");
        system(dumpcmd);
        if (once) {
            processEvents();
            ::exit(0);
        }
    }
    else {
        QRect  screenGeometry = desktop()->geometry();
        const int height = screenGeometry.height();
        const int width = screenGeometry.width();
        qInfo() << "Desktop geometry height: " << height << " width: " << width;
        QPalette palette;
        palette.setBrush(desktop()->backgroundRole(),  QBrush(*(r->getImage())));
        desktop()->setPalette(palette);
        desktop()->show();
        //XClearWindow(QX11Info::display(), QX11Info::appRootWindow());

        if (once) {
            //processEvents();
            ::exit(0);
        }
    }

    current_time = time(nullptr) + delay;
    current_time = (time_t)(start_time + (current_time - start_time) * time_warp);
    r->setTime(current_time);
    switch (p_type) {
    case SUNREL:
        r->setViewPos(r->getSunLat() + view_lat, r->getSunLong() + view_long);
        break;

    case RANDOM:
        randomPosition();
        r->setViewPos(view_lat, view_long);
        break;

    case ORBIT:
        orbitPosition(current_time);
        r->setViewPos(view_lat, view_long);
        break;

    case MOONPOS:
        MoonPos::getMoonPos(current_time, &moon_lat, &moon_long);
        r->setViewPos(moon_lat, moon_long);
        break;

    default:
        break;
    }
    r->renderFrame();
}
