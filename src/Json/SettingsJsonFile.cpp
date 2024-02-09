#include "SettingsJsonFile.h"

#include "../Logger.h"

const char* const SettingsJsonFile::_dl_path_key = "download_path";
const char* const SettingsJsonFile::_log_level_key = "log_level";

SettingsJsonFile::SettingsJsonFile(std::shared_ptr<Logger> logger) : JsonFile("config.json"), _logger(logger) {
    QJsonObject obj = get_root_obj();
    auto it = obj.find(_dl_path_key);
    const auto is_string = it != std::end(obj) && it->isString();
    _download_path = is_string ? it->toString() : QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (!is_string)
        obj[_dl_path_key] = _download_path;

    it = obj.find(_log_level_key);
    const auto is_number = it != std::end(obj) && it->isDouble();
    _log_level = is_number ? static_cast<QtMsgType>(it->toInt()) : QtWarningMsg;
    if (!is_number)
        obj[_log_level_key] = _log_level;

    _logger->set_max_level(_log_level);
    set_root_obj(obj);
}

QString SettingsJsonFile::get_download_path() const { return _download_path; }

void SettingsJsonFile::set_download_path(const QString& path) {
    if (_download_path == path)
        return;

    _download_path = path;
    set_value<const QString&>(_dl_path_key, _download_path);
}

QtMsgType SettingsJsonFile::get_max_log_level() const { return _log_level; }

void SettingsJsonFile::set_max_log_level(QtMsgType level) {
    if (_log_level == level)
        return;

    _log_level = level;
    _logger->set_max_level(_log_level);
    set_value(_log_level_key, _log_level);
}

template<typename T>
void SettingsJsonFile::set_value(const char* key, T value) {
    QJsonObject obj = get_root_obj();
    obj[key] = value;
    set_root_obj(obj);
}
