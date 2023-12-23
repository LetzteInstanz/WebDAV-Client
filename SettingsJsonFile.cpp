#include "SettingsJsonFile.h"

const char* const SettingsJsonFile::_dl_path = "download_path";

SettingsJsonFile::SettingsJsonFile() : JsonFile("config.json") {
    QJsonObject obj = get_root_obj();
    const auto it = obj.find(_dl_path);
    if (it == obj.end() || !it->isString())
        set_value(std::move(obj), QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
}

QString SettingsJsonFile::get_download_path() const {
    const QJsonObject obj = get_root_obj();
    const auto it = obj.find(_dl_path);
    return it->toString();
}

void SettingsJsonFile::set_download_path(const QString& path) { set_value(get_root_obj(), path); }

void SettingsJsonFile::set_value(QJsonObject&& obj, const QString& value) {
    obj[_dl_path] = value;
    set_root_obj(obj);
}
