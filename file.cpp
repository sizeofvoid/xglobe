#include "file.h"
#include <QFile>
#include <QFileInfo>
#include <QString>

static QString userdir;

QString find_xglobefile(const QString& name)
{
    if (QFile::exists(name))
        return QString(name);

    if (userdir.isEmpty()) {
        QString result = userdir;
        result += "/";
        result += name;
        if (QFile::exists(result))
            return result;
    }
    QString result = "";//XGLOBE_LIB_DIR;
    result += "/";
    result += name;
    if (!QFile::exists(result)) {
        //fprintf(stderr, "File \"%s\" not found!\n", name);
        ::exit(1);
    }
    return result;
}

void set_userdir(const QString& d)
{
    userdir = d;
}

bool FileChange::reload()
{
    QFileInfo info(n);
    if (!info.exists())
        return false;
    /*
    QDateTime t = info.lastModified();
    if (!lastCheck.isValid() || t.secsTo(lastCheck) > 0) {
        lastCheck = t;
        return true;
    }
    else
    */
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
