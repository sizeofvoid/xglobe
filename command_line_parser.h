#pragma once

#include <QTemporaryFile>
#include <QCommandLineParser>
#include "geo_coordinate.h"

class QCoreApplication;
class GeoCoordinate;

class CommandLineParser : public QCommandLineParser
{
public:
    CommandLineParser(QCoreApplication*);
    ~CommandLineParser() = default;

    QString getImageTmpFileName() const;
    bool isOnce() const;
    bool isKde() const;
    double getMag() const;
    double getWait() const;
    std::pair<int,int> computeLabelPosition() const;
    std::pair<int,int> computeShiftPosition() const;
    QString getMapFileName() const;
    QString getBackGFileName() const;
    TGeoCoordinatePtr getGeoCoordinate() const;
    void computeRandomPosition();
    bool isTiled() const;
    bool isStars() const;
    std::optional<int> getNice() const;
    bool isShowLabel() const;

private:
    void computeCoordinate();
    double getDoubleByValue(double, QCommandLineOption const&) const;
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
    QCommandLineOption kdeOption;
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

    TGeoCoordinatePtr coordinate;
};

