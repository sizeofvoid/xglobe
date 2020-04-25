#pragma once

#include <QDateTime>
#include <QString>

class FileChange {
public:
    FileChange(const QString&);
    ~FileChange() = default;

    bool reload();

    const QString& name() const;

    static QString findXglobeFile(const QString&);

private:
    const QString observefile;
    QDateTime lastCheck;

    static const QString default_xglobe_home_dir;
    static const QString default_xglobe_dir;
};
