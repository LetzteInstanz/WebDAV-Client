#include "Settings.h"

#include "../Qml/Sort/SortParam.h"
#include "../Json/JsonFile.h"
#include "DownloadSection.h"
#include "RootSection.h"

Settings::Settings(std::shared_ptr<Logger> logger, std::shared_ptr<JsonFile> file) : _file(std::move(file)) {
    auto get_root_object = [this]() { return _file->get_root_object(); };
    auto set_root_object = [this](nlohmann::json&& object) { _file->set_root_object(std::move(object)); };
    _root_section = std::make_unique<RootSection>(std::move(get_root_object), std::move(set_root_object), std::move(logger));
    _download_section = std::make_unique<DownloadSection>(*_root_section);
}

Settings::~Settings() = default;

bool Settings::get_ask_path_flag() const noexcept { return _download_section->get_ask_path_flag(); }

void Settings::set_ask_path_flag(bool ask) { _download_section->set_ask_path_flag(ask); }

std::filesystem::path Settings::get_download_path() const { return _download_section->get_download_path(); }

void Settings::set_download_path(std::filesystem::path&& path) { _download_section->set_download_path(std::move(path)); }

QtMsgType Settings::get_max_log_level() const noexcept { return _root_section->get_log_level(); }

void Settings::set_max_log_level(QtMsgType level) { _root_section->set_log_level(level); }

std::vector<Qml::SortParam> Settings::get_sort_params() const { return _root_section->get_sort_params(); }

void Settings::set_sort_params(const std::vector<Qml::SortParam>& params) { _root_section->set_sort_params(params); }

bool Settings::get_search_cs_flag() const noexcept { return _root_section->get_search_cs_flag(); }

void Settings::set_search_cs_flag(bool case_sensitive) { _root_section->set_search_cs_flag(case_sensitive); }

void Settings::set_sort_param_changed_notif_func(std::function<void ()>&& func) noexcept { _root_section->set_sort_param_changed_notif_func(std::move(func)); }
