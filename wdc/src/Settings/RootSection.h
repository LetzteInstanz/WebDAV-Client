#pragma once

#include "Settings.h"
#include "../Json/RootObject.h"

namespace Qml { struct SortParam; }

class Settings::RootSection final : public RootObject {
public:
    RootSection(std::function<nlohmann::json ()>&& get_root_object, std::function<void (nlohmann::json&&)>&& set_root_object);
    ~RootSection() override;

    QtMsgType get_log_level() const noexcept;
    bool set_log_level(QtMsgType level);
    std::vector<Qml::SortParam> get_sort_params() const;
    bool set_sort_params(const std::vector<Qml::SortParam>& params);
    bool get_filter_cs_flag() const noexcept;
    bool set_filter_cs_flag(bool case_sensitive);

private:
    QtMsgType _log_level;
    std::vector<Qml::SortParam> _sort_params;
    bool _case_sensitive;
};
