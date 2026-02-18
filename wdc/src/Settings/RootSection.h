#pragma once

#include "Settings.h"
#include "../Json/RootObject.h"

class Logger;
namespace Qml { struct SortParam; }

class Settings::RootSection final : public RootObject {
public:
    RootSection(std::function<nlohmann::json ()>&& get_root_object, std::function<void (nlohmann::json&&)>&& set_root_object, std::shared_ptr<Logger> logger);
    ~RootSection() override;

    QtMsgType get_log_level() const noexcept;
    void set_log_level(QtMsgType level);
    void set_sort_param_changed_notif_func(std::function<void ()>&& func) noexcept;
    std::vector<Qml::SortParam> get_sort_params() const;
    void set_sort_params(const std::vector<Qml::SortParam>& params);
    bool get_search_cs_flag() const noexcept;
    void set_search_cs_flag(bool case_sensitive);

private:
    std::shared_ptr<Logger> _logger;
    QtMsgType _log_level;
    std::vector<Qml::SortParam> _sort_params;
    bool _case_sensitive;
    std::function<void ()> _sort_param_changed_signal;
};
