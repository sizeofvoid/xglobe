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

#include <QCoreApplication>
#include <QDir>

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
      kdeOption("kde", "Use this option when the globe disappears after switching virtual screens. This is needed when using KDE."),
      starsOption("stars", "Enable displaying of stars in the background (default)."),
      nostarsOption("nostars", "Disable displaying of stars in the background."),
      posFixedOption(QStringList() << "pos-fixed", "two numerical arguments latitude and longitude  (given in decimal degrees) of a viewing position", "position"),
      posSunrelOption(QStringList() << "pos-sunrel", "Numerical arguments lat. and long., indicating offsets of current sun position.", "position", "0 0"),
      posMoonPosOption(QStringList() << "pos-moonpos", "Viewing position follows current moon position.", "position"),
      posRandomOption(QStringList() << "pos-random", "Selects a random viewing position each time a frame is redrawn.", "position"),
      posOrbitOption(QStringList() << "pos-orbit", "The position specifier keyword orbit should be followed by three arguments, interpreted as numerical values indicating the period (in hours), orbital inclination (in decimal degrees) of a simple circular orbit, and an experimental shift modifier that adjusts the orbit with each circuit; the viewing position follows this orbit.", "position"),
      dirOption(QStringList() << "dir", "Set lookup directory for files.", "dir"),
      waitOption(QStringList() << "wait", "Specifies the interval in seconds between screen updates.", "seconds", "300"),
      magOption(QStringList() << "mag", "Specifies the size of the globe in relation to the screen size. The diameter of the globe is factor times the shorter of the width and height of the screen.", "factor", "1.0"),
      rotOption(QStringList() << "rot", "A positive angle rotates the globe clockwise, a negative one counterclockwise.", "angle"),
      markerfileOption(QStringList() << "markerfile", "Load an additional location marker file. (Have a look at file \"xglobe-markers\" for reference.)", "file", ""),
      labelposOption(QStringList() << "labelpos", "Geom specifies the screen location of the label. Syntax: QStringList() << +-<xoffset>QStringList() << +-<yoffset>", "geom", "-5+5"),
      ambientlightOption(QStringList() << "ambientlight", "Indicates how the dark side of the globe appears: 0 means totally black, 100 means totally bright (= no difference between day and night side).", "level", "15"),
      ambientrgbOption(QStringList() << "ambientrgb", "Works like -ambientlight but takes 3 parameters (red, green and blue value) defining the color of ambient light. This can be useful in conjunction with a night map that is tinted towards blue, for example. Using a blueish ambient light makes the transition from day to night look better. Use either -ambientlevel or -ambientrgb, not both. (example: -ambientrgb \"1 4 20\" - This will make the night side appear blueish.)", "rgblevel", ""),
      niceOption(QStringList() << "nice", " Run the xglobe process with the given priority (see nice(1) and setpriority(2) manual pages).", "priority", ""),
      mapOption(QStringList() << "mapfile" << "map", "Use another than the default world map. Supported image formats depend on qt.", "file"),
      nightmapfileOption(QStringList() << "nightmapfile" << "nightmap" << "night", "Same as -mapfile, but for the night map.", "file", ""),
      cloudmapfileOption(QStringList() << "cloudmapfile" << "cloudmap" << "clouds", "Same as -mapfile, but for the cloud map.", "file", ""),
      cloudfilterOption(QStringList() << "cloudfilter" << "filter", "Used in conjunction with -cloudmapfile, this controls how much cloud is displayed.  n is a value between 0 and 255, where 0 will show all cloud, and 255 will only show the brightest clouds.", "n", "120"),
      dumpcmdOption(QStringList() << "dumpcmd", "Saves the rendered image to \"xglobe.bmp\" in the current directory, then executes \"cmd\", passing the image filename as an argument, eg '-dumpcmd Esetroot'.", "cmd", ""),
      outfileOption(QStringList() << "outfile", "Specifies the output file name for the -dump option.", "file", ""),
      grid1Option(QStringList() << "grid1", "Specify the spacing of major grid lines: they are drawn with 90/grid1 degree spacing. (default: -grid1 6 which corresponds to 15(o) between grid lines)", "grid1", ""),
      grid2Option(QStringList() << "grid2", "Specify spacing of dots along major grid lines. Along the equator and lines of longitude, grid dots are drawn with a 90/(grid1 x grid2) degree spacing. (default: -grid2 15 which corresponds, along with -grid1 6, to a 1(o) spacing)", "grid2", "15"),
      timewarpOption(QStringList() << "timewarp", "Scale the apparent rate at which time progresses by 'factor'.", "factor", "1.0"),
      sizeOption(QStringList() << "size", "Specify the size of the image to be rendered (useful in conjuntion with -dump). size_spec consists of two components, both positive integers. They are interpreted as the width and height (in pixels) of the image. The details provided for position specifiers (see above) about the characters used to delimit specifier components apply to size specifiers as well.(default: size of the desktop)", "size_spec", ""),
      shiftOption(QStringList() << "shift", "Specify that the center of the rendered image should be shifted by some amount from the center of the image. The spec consists of two components, both integers; these components are interpreted as the offsets (in pixels) in the X and Y directions. By default, the center of the rendered image is aligned with the center of the image.", "spec", ""),
      backgOption(QStringList() << "backg", "Use the image in file as the screen background, instead of a black screen, which is the default.", "file", ""),
      starfreqOption(QStringList() << "starfreq", "If displaying of stars is enabled, frequency percent of the background pixels are turned into stars", "frequency", "0.002"),
      termOption(QStringList() << "term", "Specify the shading discontinuity at the terminator (day/night line). Pct should be between 0 and 100, where 100 is maximum discontinuity and 0 makes a very smooth transition.", "pct", "0"),
      shade_areaOption(QStringList() << "shade_area", "Specify the proportion of the day-side to be progressively shaded prior to a transition with the night-side.  A value of 100 means all the day area will be shaded, whereas 0 will result in no shading at all.  60 would keep 40\% of the day area nearest the sun free from shading.", "pct", "100")
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
   addOption(kdeOption);
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
   addOption(dirOption);
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

    // Process the actual command line arguments given by the user
    process(*parent);

    const QStringList args = positionalArguments();
    // source is args.at(0), destination is args.at(1)

    //bool showProgress = isSet(showProgressOption);
    //bool force = isSet(forceOption);
}

QString
CommandLineParser::getImageTmpFileName()
{
    return tmpImageFile.fileName();
}
