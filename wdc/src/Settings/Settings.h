#pragma once

#include <Common/Util.h>

class JsonFile;

namespace Qml { class SortParam; }

class Settings final {
public:
    enum class Filter {CaseSensitive, Size};
    enum class Download {AskPath = to_int(Filter::Size), Path, Size};
    enum class Logging {Level = to_int(Download::Size), Size};
    enum class Sorting {Sequence = to_int(Logging::Size), Descending, Size}; // todo: write a test for intersection of the values

    Settings(std::shared_ptr<JsonFile> file);
    ~Settings();

    bool get_ask_path_flag() const noexcept;
    void set_ask_path_flag(bool ask);
    std::filesystem::path get_download_path() const;
    void set_download_path(std::filesystem::path&& path);
    QtMsgType get_max_log_level() const noexcept;
    void set_max_log_level(QtMsgType level);
    std::vector<Qml::SortParam> get_sort_params() const;
    void set_sort_params(const std::vector<Qml::SortParam>& params);
    bool get_filter_cs_flag() const noexcept;
    void set_filter_cs_flag(bool case_sensitive);

    template <typename EnumId>
    void add_notif_func_about_change(EnumId id, void* receiver, std::function<void ()>&& func);

    template <typename EnumId>
    void remove_notif_func_about_change(EnumId id, void* receiver);

private:
    template <typename EnumId>
    void call_notif_funcs(EnumId id);

private:
    class RootSection;
    class DownloadSection;

private:
    using FuncByReceiver = std::unordered_map<void*, std::function<void ()>>;
    std::unordered_map<int, FuncByReceiver> _funcs_by_id;

    std::shared_ptr<JsonFile> _file;
    std::unique_ptr<RootSection> _root_section;
    std::unique_ptr<DownloadSection> _download_section;
};

template <typename EnumId>
void Settings::add_notif_func_about_change(EnumId id, void* receiver, std::function<void ()>&& func) {
    FuncByReceiver& funcs_by_receiver = _funcs_by_id[to_int(id)];
    funcs_by_receiver.insert_or_assign(receiver, std::move(func));
}

template <typename EnumId>
void Settings::remove_notif_func_about_change(EnumId id, void* receiver) {
    const auto it = _funcs_by_id.find(to_int(id));
    if (it == std::ranges::cend(_funcs_by_id))
        return;

    auto& funcs_by_receiver = it->second;
    funcs_by_receiver.erase(receiver);
}

template <typename EnumId>
void Settings::call_notif_funcs(EnumId id) {
    const auto it = _funcs_by_id.find(to_int(id));
    if (it != std::ranges::cend(_funcs_by_id))
        std::ranges::for_each(it->second, [](std::pair<void* const, std::function<void ()>>& pair) { pair.second(); });
}
