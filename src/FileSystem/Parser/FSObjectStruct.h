#pragma once

#include "../FileSystemObject.h"

struct FSObjectStruct {
    enum class Status {Unknown, Ok, Unauthorized, Forbidden, NotFound};
    using Type = FileSystemObject::Type;

    static std::optional<Status> to_status(QStringView str);

    void replace_unknown_status(const std::optional<Status>& s);

    bool is_curr_dir_obj = false;
    std::filesystem::path path;
    std::pair<std::optional<Status>, Type> type = {{}, Type::File};
    std::pair<std::optional<Status>, std::optional<std::chrono::sys_seconds>> creation_date;
    std::pair<std::optional<Status>, std::optional<std::chrono::sys_seconds>> last_modified;
    std::pair<std::optional<Status>, std::optional<std::uint64_t>> content_length;

private:
    constexpr static std::optional<Status> ret_second_if_first_is_unknown(const std::optional<Status>& first, const std::optional<Status>& second);

private:
    static const std::vector<std::pair<QString, Status>> _str_code_pairs;
};
