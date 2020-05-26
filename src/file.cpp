#include "file.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QDebug>

const QString FileChange::default_xglobe_home_dir = QLatin1String(".xglobe");

#if defined (XGLOBE_DATA_DIR)
    const QString FileChange::default_xglobe_dir = QLatin1String(XGLOBE_DATA_DIR);
#else
    const QString FileChange::default_xglobe_dir = QLatin1String("~/xglobe");
#endif

FileChange::FileChange(const QString& filename)
    : observeFile(filename)
{
}

bool FileChange::reload()
{
    QFileInfo info(observeFile);
    if (!info.exists())
        return false;

    QDateTime t = info.lastModified();
    if (!lastCheck.isValid() || t.secsTo(lastCheck) > 0) {
        lastCheck = t;
        return true;
    }
    return false;
}

const QString& FileChange::name() const
{
    return observeFile;
}

QString FileChange::findXglobeFile(const QString& name)
{
    if (QFile::exists(name))
        return name;

    const QString homedir = QDir::homePath()
                            + QDir::separator()
                            + default_xglobe_home_dir
                            + name;

    if (QFile::exists(homedir)) {
        return homedir;
    } else {
        qWarning() << "Can't find: " << homedir;
    }

    const QString defaultdir = default_xglobe_dir
                               + QDir::separator()
                               + name;
    if (QFile::exists(defaultdir)) {
        return defaultdir;
    } else {
        qWarning() << "Can't find: " << defaultdir;
    }

    qWarning() << "Can't find map and/or marker file";
    return QString();
}
