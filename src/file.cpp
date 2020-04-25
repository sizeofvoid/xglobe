#include "file.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QString>

const QString FileChange::default_xglobe_home_dir = QLatin1String("./xglobe");

#if defined (DEFAULT_MARKER_FILE)
    const QString FileChange::default_xglobe_dir = QLatin1String(XGLOBE_DATA_DIR);
#else
    const QString FileChange::default_xglobe_dir = QLatin1String("~/xglobe");
#endif

FileChange::FileChange(const QString& filename)
    : observefile(filename)
{
}

bool FileChange::reload()
{
    QFileInfo info(observefile);
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
    return observefile;
}

QString FileChange::findXglobeFile(const QString& name)
{
    if (QFile::exists(name))
        return name;

    const QString homedir = QDir::homePath()
                            + QDir::separator()
                            + default_xglobe_home_dir
                            + QDir::separator()
                            + name;

    if (QFile::exists(homedir)) {
        return homedir;
    }

    const QString defaultdir = default_xglobe_dir + QDir::separator() + name;
    if (QFile::exists(defaultdir)) {
        return defaultdir;
    }
    return QString();
}
