#pragma once

#include <QTemporaryFile>
#include <QCommandLineParser>
#include "geo_coordinate.h"
#include "renderer.h"

class QCoreApplication;
class GeoCoordinate;
class QRgba64;

class CommandLineParser : public QCommandLineParser
{
public:
    CommandLineParser(QCoreApplication*);
    ~CommandLineParser() = default;

    QString getImageTmpFileName() const;
    bool isOnce() const;
    bool isDrawInWIndow() const;
    bool isPlasma() const;
    double getMag() const;
    double getWait() const;
    std::pair<int,int> computeLabelPosition() const;
    std::pair<int,int> computeShiftPosition() const;
    QString getBackGFileName() const;
    QString getMapFileName() const;
    TGeoCoordinatePtr getGeoCoordinate() const;
    void computeRandomPosition();
    bool isTiled() const;
    bool isStars() const;
    std::optional<int> getNice() const;
    bool isShowLabel() const;
    double getRotation() const;
    bool isShowMarker() const;
    QString getMarkerFileName() const;
    QString getMarkerFont() const;
    int getMarkerFontSize() const;
    bool isBuiltinMarkers() const;
    bool isDumpToFile() const;
    QSize getSize() const;
    int getGrid1() const;
    int getGrid2() const;
    double getTimeWrap() const;
    QString getNightMapfile() const;
    QString getCloudMapFile() const;
    int getCloudMapFilter() const;
    bool isNightmap() const;
    double getStarFreq() const;
    QRgba64 computeRgb() const;
    QString getOutputFileName() const;
    double getTransition() const;
    double getShadeArea() const;
    GridType getGridType() const;

private:
    void computeCoordinate();
    double getDoubleByValue(double, QCommandLineOption const&) const;
    int getIntByValue(int, QCommandLineOption const&) const;
    std::pair<int,int> computeXYPosition(int, int, QString const&, QCommandLineOption const&) const;

    QTemporaryFile tmpImageFile;
    QCommandLineOption onceOption;
    QCommandLineOption dumpOption;
    QCommandLineOption nightmapOption;
    QCommandLineOption nonightmapOption;
    QCommandLineOption nolabelOption;
    QCommandLineOption labelOption;
    QCommandLineOption markersOption;
    QCommandLineOption nomarkersOption;
    QCommandLineOption gridOption;
    QCommandLineOption nogridOption;
    QCommandLineOption newgridOption;
    QCommandLineOption tiledOption;
    QCommandLineOption windowOption;
    QCommandLineOption plasmaOption;
    QCommandLineOption starsOption;
    QCommandLineOption nostarsOption;
    QCommandLineOption posFixedOption;
    QCommandLineOption posSunrelOption;
    QCommandLineOption posMoonPosOption;
    QCommandLineOption posRandomOption;
    QCommandLineOption posOrbitOption;
    QCommandLineOption waitOption;
    QCommandLineOption magOption;
    QCommandLineOption rotOption;
    QCommandLineOption markerfileOption;
    QCommandLineOption labelposOption;
    QCommandLineOption ambientlightOption;
    QCommandLineOption ambientrgbOption;
    QCommandLineOption niceOption;
    QCommandLineOption mapOption;
    QCommandLineOption nightmapfileOption;
    QCommandLineOption cloudmapfileOption;
    QCommandLineOption cloudfilterOption;
    QCommandLineOption dumpcmdOption;
    QCommandLineOption outfileOption;
    QCommandLineOption grid1Option;
    QCommandLineOption grid2Option;
    QCommandLineOption timewarpOption;
    QCommandLineOption sizeOption;
    QCommandLineOption shiftOption;
    QCommandLineOption backgOption;
    QCommandLineOption starfreqOption;
    QCommandLineOption termOption;
    QCommandLineOption shade_areaOption;
    QCommandLineOption markerFontOption;
    QCommandLineOption markerFontSizeOption;

    TGeoCoordinatePtr coordinate;

#if defined (DEFAULT_MAP)
    const QString default_map = QLatin1String(DEFAULT_MAP);
#else
    const QString default_map = QLatin1String("map.png");
#endif

#if defined (DEFAULT_MAP_NIGHT)
    const QString default_map_night = QLatin1String(DEFAULT_MAP_NIGHT);
#else
    const QString default_map_night = QLatin1String("mapnight.png");
#endif

#if defined (DEFAULT_MAP_BACK)
    const QString default_map_back = QLatin1String(DEFAULT_MAP_BACK);
#else
    const QString default_map_back = QLatin1String("back.png");
#endif
};

