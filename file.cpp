#include "file.h"
#include "config.h"
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>

static const char* userdir;

QString find_xglobefile(const char* name)
{
    if (QFile::exists(name))
        return QString(name);

    if (userdir != NULL) {
        QString result = userdir;
        result += "/";
        result += name;
        if (QFile::exists(result))
            return result;
    }
    QString result = XGLOBE_LIB_DIR;
    result += "/";
    result += name;
    if (!QFile::exists(result)) {
        fprintf(stderr, "File \"%s\" not found!\n", name);
        ::exit(1);
    }
    return result;
}

void set_userdir(const char* d)
{
    userdir = d;
}

bool FileChange::reload()
{
    QFileInfo info(n);
    if (!info.exists())
        return false;
    QDateTime t = info.lastModified();
    if (!lastCheck.isValid() || t > lastCheck) {
        lastCheck = t;
        return true;
    }
    else
        return false;
}

FileChange::FileChange(const QString& filename)
    : n(filename)
{
}

const QString& FileChange::name() const
{
    return n;
}
