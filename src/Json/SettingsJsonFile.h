#pragma once

#include <memory>

#include <QJsonObject>
#include <QString>
#include <QtLogging>

#include "JsonFile.h"

class Logger;

class SettingsJsonFile final : public JsonFile {
public:
    SettingsJsonFile(std::shared_ptr<Logger> logger);

    QString get_download_path() const;
    void set_download_path(const QString& path);
    QtMsgType get_max_log_level() const;
    void set_max_log_level(const QtMsgType level);

private:
    template <typename T>
    void set_value(const char* const key, const T value);

private:
    static const char* const _dl_path_key;
    static const char* const _log_level_key;
    std::shared_ptr<Logger> _logger;
    QString _download_path;
    QtMsgType _log_level;
};
