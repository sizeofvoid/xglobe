/*
 * Copyright (c) 2020 Rafael Sadowski <rafael@sizeofvoid.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "command_line_parser.h"
#include "random.h"

#include <QCoreApplication>
#include <QDir>
#include <QRgba64>
#include <QSize>
#include <QDebug>

#include <algorithm>

CommandLineParser::CommandLineParser(QCoreApplication* parent)
    : QCommandLineParser(),
      tmpImageFile(QDir::tempPath() + "/xglobe-dump.XXXXXX.png"),
      onceOption("once", "With this option, XGlobe renders an image once and exits."),
      dumpOption("dump", "Saves the rendered image to the file specified with the -outfile option (default: \"xglobe-dump.bmp\") instead of displaying it on screen."),
      nightmapOption("nightmap", "Use default world map for the night side. The switch -ambientlight will be ignored."),
      nonightmapOption("nonightmap", "Disable use of night map."),
      nolabelOption("nolabel", "Disable displaying of label (default: -label)"),
      labelOption("label", "Enable displaying of label indicating current date and time etc."),
      markersOption("markers", "Enable displaying of built-in display markers."),
      nomarkersOption("nomarkers", "Disable displaying of built-in display markers (default: -marker)"),
      gridOption("grid", "Enable displaying of grid on the globe."),
      nogridOption("nogrid", "Disable displaying of grid on the globe."),
      newgridOption("newgrid", "Enable displaying of a more fancy grid."),
      tiledOption("tiled", "The background image specified using -backg is by default expanded to fill the screen. This option will cause it to be tiled instead."),
      windowOption("window", "The globe is drawn in a window instead of a wallpaper."),
      plasmaOption("plasma", "Enable xglobe KDE Plasma support."),
      starsOption("stars", "Enable displaying of stars in the background (default)."),
      nostarsOption("nostars", "Disable displaying of stars in the background."),
      posFixedOption(QStringList() << "pos-fixed", "two numerical arguments latitude and longitude  (given in decimal degrees) of a viewing position", "position"),
      posSunrelOption(QStringList() << "pos-sunrel", "Numerical arguments lat. and long., indicating offsets of current sun position.", "position", "0 0"),
      posMoonPosOption(QStringList() << "pos-moonpos", "Viewing position follows current moon position.", "position"),
      posRandomOption(QStringList() << "pos-random", "Selects a random viewing position each time a frame is redrawn.", "position"),
      posOrbitOption(QStringList() << "pos-orbit", "The position specifier keyword orbit should be followed by three arguments, interpreted as numerical values indicating the period (in hours), orbital inclination (in decimal degrees) of a simple circular orbit, and an experimental shift modifier that adjusts the orbit with each circuit; the viewing position follows this orbit.", "position"),
      waitOption(QStringList() << "wait", "Specifies the interval in seconds between screen updates.", "seconds", "3"),
      magOption(QStringList() << "mag", "Specifies the size of the globe in relation to the screen size. The diameter of the globe is factor times the shorter of the width and height of the screen.", "factor", "1.0"),
      rotOption(QStringList() << "rot", "A positive angle rotates the globe clockwise, a negative one counterclockwise.", "angle"),
      markerfileOption(QStringList() << "markerfile", "Load an additional location marker file. (Have a look at file \"xglobe-markers\" for reference.)", "file", ""),
      labelposOption(QStringList() << "labelpos", "Geom specifies the screen location of the label. Syntax: +-<xoffset>:+-<yoffset>", "geom", "-5:+5"),
      ambientlightOption(QStringList() << "ambientlight", "Indicates how the dark side of the globe appears: 0 means totally black, 100 means totally bright (= no difference between day and night side).", "level", "15"),
      ambientrgbOption(QStringList() << "ambientrgb", "Works like -ambientlight but takes 3 parameters (red, green and blue value) defining the color of ambient light. This can be useful in conjunction with a night map that is tinted towards blue, for example. Using a blueish ambient light makes the transition from day to night look better. Use either -ambientlevel or -ambientrgb, not both. (example: -ambientrgb \"1 4 20\" - This will make the night side appear blueish.)", "rgblevel", ""),
      niceOption(QStringList() << "nice", " Run the xglobe process with the given priority (see nice(1) and setpriority(2) manual pages).", "priority", ""),
      mapOption(QStringList() << "mapfile" << "map", "Use another than the default world map. Supported image formats depend on qt.", "file"),
      nightmapfileOption(QStringList() << "nightmapfile" << "night", "Same as -mapfile, but for the night map.", "file", ""),
      cloudmapfileOption(QStringList() << "cloudmapfile" << "cloudmap" << "clouds", "Same as -mapfile, but for the cloud map.", "file", ""),
      cloudfilterOption(QStringList() << "cloudfilter" << "filter", "Used in conjunction with -cloudmapfile, this controls how much cloud is displayed.  n is a value between 0 and 255, where 0 will show all cloud, and 255 will only show the brightest clouds.", "n", "120"),
      dumpcmdOption(QStringList() << "dumpcmd", "Saves the rendered image to \"xglobe.bmp\" in the current directory, then executes \"cmd\", passing the image filename as an argument, eg '-dumpcmd Esetroot'.", "cmd", ""),
      outfileOption(QStringList() << "outfile", "Specifies the output file name for the -dump option.", "file", ""),
      grid1Option(QStringList() << "grid1", "Specify the spacing of major grid lines: they are drawn with 90/grid1 degree spacing. (default: -grid1 6 which corresponds to 15(o) between grid lines)", "grid1", ""),
      grid2Option(QStringList() << "grid2", "Specify spacing of dots along major grid lines. Along the equator and lines of longitude, grid dots are drawn with a 90/(grid1 x grid2) degree spacing. (default: -grid2 15 which corresponds, along with -grid1 6, to a 1(o) spacing)", "grid2", "15"),
      timewarpOption(QStringList() << "timewarp", "Scale the apparent rate at which time progresses by 'factor'.", "factor", "1.0"),
      sizeOption(QStringList() << "size", "Specify the size of the image to be rendered (useful in conjuntion with -dump). size_spec consists of two components, both positive integers. They are interpreted as the width and height (in pixels) of the image. The details provided for position specifiers (see above) about the characters used to delimit specifier components apply to size specifiers as well. (default: size of the desktop)", "size_spec", ""),
      shiftOption(QStringList() << "shift", "Specify that the center of the rendered image should be shifted by some amount from the center of the image. The spec consists of two components, both integers; these components are interpreted as the offsets (in pixels) in the X and Y directions. By default, the center of the rendered image is aligned with the center of the image.", "spec", ""),
      backgOption(QStringList() << "backg", "Use the image in file as the screen background, instead of a black screen, which is the default.", "file", ""),
      starfreqOption(QStringList() << "starfreq", "If displaying of stars is enabled, frequency percent of the background pixels are turned into stars", "frequency", "0.002"),
      termOption(QStringList() << "term", "Specify the shading discontinuity at the terminator (day/night line). Pct should be between 0 and 100, where 100 is maximum discontinuity and 0 makes a very smooth transition.", "pct", "0"),
      shade_areaOption(QStringList() << "shade_area", "Specify the proportion of the day-side to be progressively shaded prior to a transition with the night-side.  A value of 100 means all the day area will be shaded, whereas 0 will result in no shading at all.  60 would keep 40\% of the day area nearest the sun free from shading.", "pct", "100"),
      markerFontOption(QStringList() << "markerfont", "", "font", "helvetica"),
      markerFontSizeOption(QStringList() << "markerfontsize", "", "fontsize", "12"),
      xwallpaperOption(QStringList() << "xwallpaper-opt",
                       QString::fromLatin1("xwallpaper options. If the argument string contains an ")
                                           + xwallpaprer_image_tag
                                           + QString::fromLatin1(" then it will be replaced by the path to the image."),
                       "xwallpaper-args",
                       QString::fromLatin1("--zoom ") + xwallpaprer_image_tag)
{
   tmpImageFile.open();
   setApplicationDescription("XGlobe");
   addHelpOption();
   addVersionOption();

   // A boolean option with a single name
   addOption(onceOption);
   addOption(dumpOption);
   addOption(nightmapOption);
   addOption(nonightmapOption);
   addOption(nolabelOption);
   addOption(labelOption);
   addOption(markersOption);
   addOption(nomarkersOption);
   addOption(gridOption);
   addOption(nogridOption);
   addOption(newgridOption);
   addOption(tiledOption);
   addOption(windowOption);
   addOption(plasmaOption);
   addOption(starsOption);
   addOption(nostarsOption);

    // Options with a value
   //- positive lat. - north of equator
   //- negative lat. - south of equator
   //- positive long. - east of prime meridian
   //- negative long. - west of prime meridian
   addOption(posFixedOption);
   addOption(posSunrelOption);
   addOption(posMoonPosOption);
   addOption(posRandomOption);
   addOption(posOrbitOption);
   addOption(waitOption);
   addOption(magOption);
   addOption(rotOption);
   addOption(markerfileOption);
   addOption(labelposOption);
   addOption(ambientlightOption);
   addOption(ambientrgbOption);
   addOption(niceOption);
   addOption(mapOption);
   addOption(nightmapfileOption);
   addOption(cloudmapfileOption);
   addOption(cloudfilterOption);
   addOption(dumpcmdOption);
   addOption(outfileOption);
   addOption(grid1Option);
   addOption(grid2Option);
   addOption(timewarpOption);
   addOption(sizeOption);
   addOption(shiftOption);
   addOption(backgOption);
   addOption(starfreqOption);
   addOption(termOption);
   addOption(shade_areaOption);
   addOption(markerFontOption);
   addOption(markerFontSizeOption);
   addOption(xwallpaperOption);

    // Process the actual command line arguments given by the user
    process(*parent);

    const QStringList args = positionalArguments();
    // source is args.at(0), destination is args.at(1)

    //bool showProgress = isSet(showProgressOption);
    //bool force = isSet(forceOption);
    computeCoordinate();
}

QString
CommandLineParser::getImageTmpFileName() const
{
    return tmpImageFile.fileName();
}

bool
CommandLineParser::isOnce() const
{
    return isSet(onceOption);
}

bool
CommandLineParser::isDrawInWIndow() const
{
    return isSet(windowOption);
}

bool
CommandLineParser::isPlasma() const
{
    return isSet(plasmaOption);
}

double
CommandLineParser::getDoubleByValue(double defaultValue, QCommandLineOption const& option) const
{
    if (!isSet(option))
        return defaultValue;
    QString s = value(option);
    bool ok;
    const double d = s.toDouble(&ok);
    return ok ? d : defaultValue;
}

int
CommandLineParser::getIntByValue(int defaultValue, QCommandLineOption const& option) const
{
    if (!isSet(option))
        return defaultValue;
    QString s = value(option);
    bool ok;
    const int d = s.toInt(&ok);
    return ok ? d : defaultValue;
}

double
CommandLineParser::getWait() const
{
    return getDoubleByValue(3, waitOption);
}

double
CommandLineParser::getMag() const
{
    return getDoubleByValue(1.0, magOption);
}

QString
CommandLineParser::getMapFileName() const
{
    if (!isSet(mapOption)) {
        qWarning() << "Fallback map: " << default_map;
        return default_map;
    }

    const QString mapFile = value(mapOption);
    const bool exists = QFile::exists(mapFile);
    if (!exists)
        qWarning() << "Mapfile not exists: " << mapFile;

    return exists ? mapFile : default_map;
}

QString
CommandLineParser::getBackGFileName() const
{
    if (!isSet(backgOption)) {
        return QString();
    }

    const QString backGFile = value(backgOption);
    const bool exists = QFile::exists(backGFile);
    if (!exists)
        qWarning() << "backGFile not exists: " << backGFile;

    return exists ? backGFile : default_map_back;
}

QString
CommandLineParser::getMarkerFileName() const
{
    const QString markerFileName = value(markerfileOption);
    const bool exists = QFile::exists(markerFileName);
    if (!exists)
        qWarning() << "Markerfile not exists: " << markerFileName;

    return exists ? markerFileName : QString();
}

QString
CommandLineParser::getMarkerFont() const
{
    if (!isSet(markerFontOption))
        return {};
    return value(markerFontOption);
}

int
CommandLineParser::getMarkerFontSize() const
{
    const int size = getIntByValue(5, markerFontSizeOption);
    return (size >= 50) ? 50 : size;
}

void
CommandLineParser::computeCoordinate()
{
    auto getLatLon = [&]() -> std::vector<double>
    {
        QString val;
        size_t maxValues = 0;
        if (isSet(posFixedOption)) {
            val = value(posFixedOption);
            maxValues = 2;
        }
        else if(isSet(posSunrelOption)) {
            val = value(posSunrelOption);
            maxValues = 2;
        }
        else if(isSet(posMoonPosOption)) {
            val = value(posMoonPosOption);
            maxValues = 2;
        }
        else if(isSet(posOrbitOption) ) {
            val = value(posOrbitOption);
            maxValues = 3;
        }

        auto vals = val.splitRef(QLatin1String(";"));
        if (vals.isEmpty() || vals.size() != 2) {
            qWarning() << "Invalid syntax for position option. Use \"latitude;longitude\", exmaple \"11.5;23.0\"";
            return {};
        }

        std::vector<double> vecVals;
        vecVals.reserve(maxValues);
        for (size_t i= 0; i < maxValues; i++) {
            if (vals.at(i).string()) {
                bool ok = false;
                const double d = vals.at(0).toDouble(&ok);
                if (!ok)
                    throw std::logic_error("QString::toDouble");
                vecVals.push_back(d);
            }
        }
        assert(vecVals.size() == maxValues);
        return vecVals;
    };

    int posOptions = (isSet(posFixedOption) ? 1 : 0)
          + (isSet(posSunrelOption) ? 1 : 0)
          + (isSet(posMoonPosOption) ? 1 : 0)
          + (isSet(posRandomOption) ? 1 : 0)
          + (isSet(posOrbitOption) ? 1 : 0);

    if (posOptions == 0) {
        coordinate = std::make_shared<GeoCoordinate>(0, 0);
        return;
    }

    if (posOptions > 1) {
        qWarning() << "Only one position option is allowed. Use pos-{fixed|sunrel|moonpos|random|orbit}. There is none used!";
        coordinate = std::make_shared<GeoCoordinate>(0, 0);
        return;
    }

    if (isSet(posRandomOption)) {
        coordinate = std::make_shared<GeoCoordinate>(0, 0, PosType::random);
        computeRandomPosition();
        return;
    }

    auto lat_lon = getLatLon();

    if (isSet(posFixedOption)) {
        coordinate = std::make_shared<GeoCoordinate>(lat_lon.at(0), lat_lon.at(1), PosType::fixed);
        return;
    }
    else if(isSet(posSunrelOption)) {
        coordinate = std::make_shared<GeoCoordinate>(lat_lon.at(0), lat_lon.at(1), PosType::sunrel);
        return;
    }
    else if(isSet(posMoonPosOption)) {
        coordinate = std::make_shared<GeoCoordinate>(lat_lon.at(0), lat_lon.at(1), PosType::moonpos);
        return;
    }
    else if(isSet(posOrbitOption) ) {

        auto orbit_period = lat_lon.at(0) * 3600;
        auto orbit_inclin = lat_lon.at(1);
        auto orbit_shift = lat_lon.at(2);

        if (orbit_period <= 0) {
            qCritical("orbit period must be a positive number.\n");
        }
        if (orbit_inclin > 90 || orbit_inclin < -90) {
            qCritical("orbit inclination must be between -90 and 90\n");
        }
        if (orbit_shift < 0) {
            qCritical("orbit shift must be larger than or equal to zero\n");
        }
        coordinate = std::make_shared<OrbitCoordinate>(orbit_period, orbit_inclin, orbit_shift);
        return;
    }
}

std::pair<int,int>
CommandLineParser::computeLabelPosition() const
{
    return computeXYPosition(-5, 5, "Invalid syntax for position option. Use \"+-<xoffset>:+-<yoffset>\", exmaple \"-11:+23\"", labelposOption);
}

std::pair<int,int>
CommandLineParser::computeShiftPosition() const
{
    return computeXYPosition(0, 0, "Invalid syntax for position option. Use \"+-<X>:+-<Y>\", exmaple \"-11:+23\"", shiftOption);
}

std::pair<int,int>
CommandLineParser::computeXYPosition(int default_x, int default_y, QString const& warn, QCommandLineOption const& option) const
{
    if (!isSet(option)) {
        return {default_x, default_y};
    }

    QString val = value(labelposOption);
    auto vals = val.splitRef(QLatin1String(":"));
    if (vals.isEmpty() || vals.size() != 2) {
        qWarning() <<warn;
        return {default_x, default_y};
    }

    bool ok = false;
    int x = vals.at(0).toInt(&ok);
    if (!ok)
        throw std::logic_error("QString::toDouble");

    int y = vals.at(1).toInt(&ok);
    if (!ok)
        throw std::logic_error("QString::toDouble");

    qDebug() << "positions x: " << x << " y: " << y;
    return {x, y};
}

TGeoCoordinatePtr
CommandLineParser::getGeoCoordinate() const
{
    return coordinate;
}

void
CommandLineParser::computeRandomPosition()
{
    Gen gen;
    coordinate->setLatitude((gen(30001) / 30000.) * 180. - 90.);
    coordinate->setLongitude((gen(30001) / 30000.) * 360. - 180.);
}

bool
CommandLineParser::isTiled() const
{
    return isSet(tiledOption);
}

bool
CommandLineParser::isStars() const
{
    return (isSet(starsOption) && getBackGFileName().isEmpty()) && !isSet(nostarsOption);
}

std::optional<int>
CommandLineParser::getNice() const
{
    if (isSet(niceOption))
    {
        QString val = value(niceOption);
        bool ok = false;
        int x = val.toInt(&ok);
        if (ok)
            return x;
    }
    return {};
}

bool
CommandLineParser::isShowLabel() const
{
    return isSet(labelOption) && !isSet(labelOption);
}

double
CommandLineParser::getRotation() const
{
    return getDoubleByValue(0, rotOption);
}

bool
CommandLineParser::isShowMarker() const
{
    return isSet(markersOption) && isSet(markerfileOption);
}

bool
CommandLineParser::isBuiltinMarkers() const
{
    return isSet(markersOption) && !isSet(nomarkersOption) && !isSet(markerfileOption);
}

bool
CommandLineParser::isDumpToFile() const
{
    return isSet(dumpOption);
}

QSize
CommandLineParser::getSize() const
{
    if (!isSet(sizeOption)) {
        return {};
    }

    QString val = value(sizeOption);
    auto vals = val.splitRef(QLatin1String(":"));
    if (vals.isEmpty() || vals.size() != 2) {
        return {};
    }

    bool ok = false;
    int width = vals.at(0).toInt(&ok);
    if (!ok)
        throw std::logic_error("QString::toDouble");

    int height = vals.at(1).toInt(&ok);
    if (!ok)
        throw std::logic_error("QString::toDouble");

    qDebug() << "positions width: " << width << " height: " << height;
    return {width, height};
}

int
CommandLineParser::getGrid1() const
{
    return getIntByValue(6, grid1Option);
}

int
CommandLineParser::getGrid2() const
{
    return getIntByValue(15, grid2Option);
}

double
CommandLineParser::getTimeWrap() const
{
    return getDoubleByValue(15, timewarpOption);
}

QString
CommandLineParser::getNightMapfile() const
{
    if (!isSet(nightmapfileOption))
        return QString();

    const QString nightmapfile = value(nightmapfileOption);
    const bool exists = QFile::exists(nightmapfile);
    if (!exists)
        qWarning() << "nightmapfile not exists: " << nightmapfile;

    return exists ? nightmapfile : default_map_night;
}

QString
CommandLineParser::getCloudMapFile() const
{
    if (!isSet(cloudmapfileOption))
        return {};
    const QString cloudmapfile = value(cloudmapfileOption);
    const bool exists = QFile::exists(cloudmapfile);
    if (!exists)
        qWarning() << "cloudmapfile not exists: " << cloudmapfile;

    return exists ? cloudmapfile : QString();
}

int
CommandLineParser::getCloudMapFilter() const
{
    return getIntByValue(120, cloudfilterOption);
}

bool
CommandLineParser::isNightmap() const
{
    return isSet(nightmapOption) || isSet(nonightmapOption);
}

double
CommandLineParser::getStarFreq() const
{
    return getDoubleByValue(0.1, starfreqOption);
}

QRgba64
CommandLineParser::computeRgb() const
{
    const auto defaultRgb = QRgba64::fromRgba64(15, 15, 15, 0);
    auto adjustAmbientlight = [](double& a) {
        if (a > 100.)
            a = 100.;
        else if (a < 0.)
            a = 0.;
        a /= 100.;
    };

    if (isSet(ambientlightOption))
    {
        double ambientlight = getDoubleByValue(-1, ambientlightOption);
        adjustAmbientlight(ambientlight);
        return QRgba64::fromRgba64(ambientlight, ambientlight, ambientlight, 0);
    }
    else if (isSet(ambientrgbOption)) {
        QString val = value(ambientrgbOption);
        auto vals = val.splitRef(QLatin1String(":"));
        if (vals.isEmpty() || vals.size() != 3) {
            return defaultRgb;
        }

        bool ok = false;
        double r = vals.at(0).toDouble(&ok);
        if (!ok)
            throw std::logic_error("QString::toDouble");

        double g = vals.at(1).toDouble(&ok);
        if (!ok)
            throw std::logic_error("QString::toDouble");

        double b = vals.at(1).toDouble(&ok);
        if (!ok)
            throw std::logic_error("QString::toDouble");

        adjustAmbientlight(r);
        adjustAmbientlight(g);
        adjustAmbientlight(b);
        return QRgba64::fromRgba64(r, g, b, 0);
    }
    return defaultRgb;
}

QString
CommandLineParser::getOutputFileName() const
{
    return value(outfileOption);
}

double
CommandLineParser::getTransition() const
{
    double transition = getDoubleByValue(0.0, termOption);
    if((transition < 0.0) || (transition > 100.0))
        qCritical("transition should be >0 and <100.\n");
    transition /= 100.0;
    return transition;
}

double
CommandLineParser::getShadeArea() const
{
    double shade_area = getDoubleByValue(3, waitOption);
    if ((shade_area < 0.0) || (shade_area > 100.0)) {
        qCritical("shade_areashould be >0 and <100.\n");
    }
    shade_area /= 100.0;
    return shade_area;
}

GridType
CommandLineParser::getGridType() const
{
    if (isSet(nogridOption))
        return GridType::no;

    if (isSet(gridOption))
        return GridType::no;

    if (isSet(newgridOption))
        return GridType::nice;
    return GridType::no;
}

/**
 * @param image which replace XIMAGE (@see xwallpaprer_image_tag)
 * @return List of all xwallpaper options default (@see xwallpaperOption) or
 * set by the user.
 */
QStringList
CommandLineParser::getXWallpaperOptions(QString const& image) const
{
    QString options = value(xwallpaperOption);
    options.replace(xwallpaprer_image_tag, image);
    return options.split(QLatin1Char(' '));;
}
