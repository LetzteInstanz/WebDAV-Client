#include "SettingsJsonFile.h"

const char* const SettingsJsonFile::_dl_path = "download_path";

SettingsJsonFile::SettingsJsonFile() : JsonFile("config.json") {}

QString SettingsJsonFile::get_download_path() const {
    const QJsonObject obj = get_root_obj();
    const auto it = obj.find(_dl_path);
    return it == obj.end() || !it->isString() ? QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) : it->toString();
}

void SettingsJsonFile::set_download_path(const QString& path) {
    const QJsonObject obj = get_root_obj();
    obj[_dl_path] = path;
    write(obj);
}
