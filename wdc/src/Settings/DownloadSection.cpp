#include "DownloadSection.h"

namespace JsonKeys {
    const constexpr char* object = "downloading";
    const constexpr char* ask_path = "ask_path";
    const constexpr char* path = "path";
}

Settings::DownloadSection::DownloadSection(JsonObject& parent) : JsonObject(&parent, JsonKeys::object) {
    nlohmann::json json_object = get_json_object();
    auto needs_to_update = false;
    bool value_changed;
    _download_path = *get_value<std::filesystem::path>(json_object, []() { return std::filesystem::path(); }, JsonKeys::path, value_changed);
    needs_to_update |= value_changed;

    _ask_path = *get_value<bool>(json_object, []() { return true; }, JsonKeys::ask_path, value_changed);
    needs_to_update |= value_changed;

    if (_download_path.empty() && !_ask_path) {
        _ask_path = true;
        json_object[JsonKeys::ask_path] = _ask_path;
        needs_to_update = true;
        qWarning("%s", qUtf8Printable(QObject::tr("The download path cannot be empty with \"ask_path\" false")));
    }
    if (!_ask_path) {
        assert(!_download_path.empty());
        try {
            std::filesystem::create_directory(_download_path);
        } catch (const std::filesystem::filesystem_error& e) {
            qCritical(qUtf8Printable(QObject::tr("Could not create the default download directory: %s")), e.what());
        }
    }
    if (needs_to_update)
        set_json_object(std::move(json_object));
}

bool Settings::DownloadSection::get_ask_path_flag() const noexcept { return _ask_path; }

bool Settings::DownloadSection::set_ask_path_flag(bool ask) { return set_value(JsonKeys::ask_path, _ask_path, ask); }

std::filesystem::path Settings::DownloadSection::get_download_path() const { return _download_path; }

bool Settings::DownloadSection::set_download_path(std::filesystem::path&& path) { return set_value(JsonKeys::path, _download_path, path); }
