#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <QJsonArray>
#include <QString>
#include <QStringView>
#include <QtLogging>

#include "JsonFile.h"

class Logger;

namespace Qml {
    enum class FileItemModelRole;

    class SortParam;
}

class SettingsJsonFile final : public JsonFile {
public:
    SettingsJsonFile(std::shared_ptr<Logger> logger);

    QString get_download_path() const;
    void set_download_path(const QStringView& path);
    QtMsgType get_max_log_level() const;
    void set_max_log_level(QtMsgType level);
    std::vector<Qml::SortParam> get_sort_params() const;
    void set_sort_params(const std::vector<Qml::SortParam>& params);

private:
    using SortParamVector = std::vector<Qml::SortParam>;

    static SortParamVector get_default_sort_params();
    bool read_sort_params(const QJsonArray& array);
    static QJsonArray to_json_array(const SortParamVector& params);
    template <typename T>
    void set_value(const char* key, T&& value);

private:
    static const char* const _dl_path_key;
    static const char* const _log_level_key;
    static const char* const _sort_param_array_key;
    static const char* const _sort_param_id_key;
    static const char* const _sort_param_desc_key;
    static const std::unordered_map<QString, Qml::SortParam> _supported_sort_params;
    static const std::vector<QString> _default_sort_param_order;
    static std::unordered_map<Qml::FileItemModelRole, QString> _sort_param_json_id_by_role_map;
    std::shared_ptr<Logger> _logger;
    QString _download_path;
    QtMsgType _log_level;
    SortParamVector _sort_params;
};
