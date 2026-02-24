#include "Settings.h"

#include "../Qml/Sort/SortParam.h"
#include "../Json/JsonFile.h"
#include "DownloadSection.h"
#include "RootSection.h"

Settings::Settings(std::shared_ptr<JsonFile> file) : _file(std::move(file)) {
    auto get_root_object = [this]() { return _file->get_root_object(); };
    auto set_root_object = [this](nlohmann::json&& object) { _file->set_root_object(std::move(object)); };
    _root_section = std::make_unique<RootSection>(std::move(get_root_object), std::move(set_root_object));
    _download_section = std::make_unique<DownloadSection>(*_root_section);
}

Settings::~Settings() = default;

bool Settings::get_ask_path_flag() const noexcept { return _download_section->get_ask_path_flag(); }

void Settings::set_ask_path_flag(bool ask) {
    if (_download_section->set_ask_path_flag(ask))
        call_notif_funcs(Download::AskPath);
}

std::filesystem::path Settings::get_download_path() const { return _download_section->get_download_path(); }

void Settings::set_download_path(std::filesystem::path&& path) {
    if (_download_section->set_download_path(std::move(path)))
        call_notif_funcs(Download::Path);
}

QtMsgType Settings::get_max_log_level() const noexcept { return _root_section->get_log_level(); }

void Settings::set_max_log_level(QtMsgType level) {
    if (_root_section->set_log_level(level))
        call_notif_funcs(Logging::Level);
}

std::vector<Qml::SortParam> Settings::get_sort_params() const { return _root_section->get_sort_params(); }

void Settings::set_sort_params(const std::vector<Qml::SortParam>& params) {
    if (_root_section->set_sort_params(params))
        call_notif_funcs(Sorting::Sequence);
}

bool Settings::get_filter_cs_flag() const noexcept { return _root_section->get_filter_cs_flag(); }

void Settings::set_filter_cs_flag(bool case_sensitive) {
    if (_root_section->set_filter_cs_flag(case_sensitive))
        call_notif_funcs(Filter::CaseSensitive);
}
