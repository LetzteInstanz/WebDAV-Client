#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

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

    std::string get_download_path() const;
    void set_download_path(std::string_view path);
    QtMsgType get_max_log_level() const noexcept;
    void set_max_log_level(QtMsgType level);
    std::vector<Qml::SortParam> get_sort_params() const;
    void set_sort_params(const std::vector<Qml::SortParam>& params);
    bool get_search_cs_flag() const noexcept;
    void set_search_cs_flag(bool case_sensitive);
    void set_notification_func(std::function<void ()>&& func) noexcept;

private:
    using SortParamVector = std::vector<Qml::SortParam>;

    static SortParamVector get_default_sort_params();
    bool read_sort_params(const nlohmann::json& array);
    static nlohmann::json to_json_array(const SortParamVector& params);
    template <typename T>
    void set_value(const char* key, T&& value);

private:
    static const constexpr char* _dl_path_key = "download_path";
    static const constexpr char* _log_level_key = "log_level";
    static const constexpr char* _sort_param_array_key = "sort";
    static const constexpr char* _sort_param_id_key = "id";
    static const constexpr char* _sort_param_desc_key = "descending";
    static const constexpr char* _cs_key = "case_sensitive";
    static const std::unordered_map<std::string, Qml::SortParam> _supported_sort_params;
    static const std::vector<std::string> _default_sort_param_order;
    static std::unordered_map<Qml::FileItemModelRole, std::string> _sort_param_json_id_by_role_map;
    std::shared_ptr<Logger> _logger;
    std::string _download_path;
    QtMsgType _log_level;
    SortParamVector _sort_params;
    bool _case_sensitive;
    std::function<void ()> _sort_param_changed_signal;
};
