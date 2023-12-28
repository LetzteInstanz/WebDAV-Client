#include "SettingsJsonFile.h"

#include "Logger.h"

const char* const SettingsJsonFile::_dl_path_key = "download_path";
const char* const SettingsJsonFile::_log_level_key = "log_level";

SettingsJsonFile::SettingsJsonFile(std::shared_ptr<Logger> logger) : JsonFile("config.json"), _logger(logger) {
    QJsonObject obj = get_root_obj();
    auto it = obj.find(_dl_path_key);
    if (it == obj.end() || !it->isString())
        obj[_dl_path_key] = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

    it = obj.find(_log_level_key);
    if (it == obj.end() || !it->isDouble())
        set_max_log_level(obj, QtWarningMsg);
}

QString SettingsJsonFile::get_download_path() const {
    const QJsonObject obj = get_root_obj();
    const auto it = obj.find(_dl_path_key);
    return it->toString();
}

void SettingsJsonFile::set_download_path(const QString& path) { set_value<const QString&>(_dl_path_key, path); }

QtMsgType SettingsJsonFile::get_max_log_level() const {
    const QJsonObject obj = get_root_obj();
    const auto it = obj.find(_log_level_key);
    return static_cast<QtMsgType>(it->toInt());
}

void SettingsJsonFile::set_max_log_level(const QtMsgType level) {
    _logger->set_max_level(level);
    set_value(_log_level_key, level);
}

template<typename T>
void SettingsJsonFile::set_value(const char* const key, const T value) {
    QJsonObject obj = get_root_obj();
    obj[key] = value;
    set_root_obj(obj);
}

void SettingsJsonFile::set_max_log_level(QJsonObject& obj, const QtMsgType level) {
    _logger->set_max_level(level);
    obj[_log_level_key] = level;
}
