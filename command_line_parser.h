#pragma once

#include <QTemporaryFile>
#include <QCommandLineParser>

class QCoreApplication;

class CommandLineParser : public QCommandLineParser
{
public:
    CommandLineParser(QCoreApplication*);
    ~CommandLineParser() = default;

    QString getImageTmpFileName() const;
    bool isOnce() const;
    bool isKde() const;
    double getMag() const;
    QString getMapFileName() const;

private:
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
    QCommandLineOption dirOption;
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
};

