#pragma once

#include <QDateTime>
#include <QString>

QString find_xglobefile(const char*);
void set_userdir(const char*);

class FileChange {
public:
    FileChange(const QString&);
    bool reload();
    const QString& name() const;

private:
    const QString n;
    QDateTime lastCheck;
};
