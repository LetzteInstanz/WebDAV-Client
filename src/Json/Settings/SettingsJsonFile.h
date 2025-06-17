#pragma once

#include <functional>
#include <memory>
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

private:
    std::shared_ptr<Logger> _logger;
    std::string _download_path;
    QtMsgType _log_level;
    SortParamVector _sort_params;
    bool _case_sensitive;
    std::function<void ()> _sort_param_changed_signal;
};
