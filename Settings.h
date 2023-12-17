#pragma once

#include <QFile>
#include <QString>

class Settings {
public:
    Settings();

    QString get_download_path() const { return _download_path; }
    void set_download_path(const QString& path) { _download_path = path; }

private:
    const QString _appDirName;
    QFile _file;
    QString _download_path;
};
