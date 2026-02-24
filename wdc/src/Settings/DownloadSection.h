#pragma once

#include "Settings.h"
#include "../Json/JsonObject.h"

class Settings::DownloadSection final : public JsonObject {
public:
    DownloadSection(JsonObject& parent);

    bool get_ask_path_flag() const noexcept;
    bool set_ask_path_flag(bool ask);
    std::filesystem::path get_download_path() const;
    bool set_download_path(std::filesystem::path&& path);

private:
    bool _ask_path;
    std::filesystem::path _download_path;
};
