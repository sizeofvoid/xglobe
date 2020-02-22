#pragma once
#include <qdatetime.h>

class QString;
extern QString find_xglobefile(const char*);
extern void set_userdir(const char*);

class FileChange {
public:
    FileChange(const QString&);
    bool reload();
    const QString& name() const;

private:
    const QString n;
    QDateTime lastCheck;
};
