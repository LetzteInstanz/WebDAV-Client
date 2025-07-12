#pragma once

class Logger;
class JsonFile;

namespace Qml { class SortParam; }

class Settings final {
public:
    Settings(std::shared_ptr<Logger> logger, std::shared_ptr<JsonFile> file);
    ~Settings();

    bool get_ask_path_flag() const noexcept;
    void set_ask_path_flag(bool ask);
    std::filesystem::path get_download_path() const;
    void set_download_path(std::filesystem::path&& path);
    QtMsgType get_max_log_level() const noexcept;
    void set_max_log_level(QtMsgType level);
    std::vector<Qml::SortParam> get_sort_params() const;
    void set_sort_params(const std::vector<Qml::SortParam>& params);
    bool get_search_cs_flag() const noexcept;
    void set_search_cs_flag(bool case_sensitive);
    void set_sort_param_changed_notif_func(std::function<void ()>&& func) noexcept;

private:
    class RootSection;
    class DownloadSection;

private:
    std::shared_ptr<JsonFile> _file;
    std::unique_ptr<RootSection> _root_section;
    std::unique_ptr<DownloadSection> _download_section;
};
