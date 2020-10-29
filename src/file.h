#pragma once

#include <QDateTime>
#include <QString>

class FileChange {
public:
    FileChange(const QString&);

    bool reload();

    const QString& name() const;

    static QString findXglobeFile(const QString&);
    static QString getHomePath();
    static QString getDefaultPath();

private:
    const QString observeFile;
    QDateTime lastCheck;

    static const QString default_xglobe_home_dir;
    static const QString default_xglobe_dir;
};
