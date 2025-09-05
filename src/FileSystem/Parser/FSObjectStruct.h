#pragma once

#include "../FileSystemObject.h"

struct FSObjectStruct {
    enum class Status : std::uint16_t {Ok = 200, Unknown = std::numeric_limits<std::uint16_t>::max()}; // note: may contain any HTTP status code
    using Type = FileSystemObject::Type;

    static std::optional<Status> to_status(QStringView str);

    void replace_unknown_status(const std::optional<Status>& s);

    std::optional<std::filesystem::path> path;
    std::pair<std::optional<Status>, std::optional<Type>> type;
    std::pair<std::optional<Status>, std::optional<std::chrono::sys_seconds>> creation_date;
    std::pair<std::optional<Status>, std::optional<std::chrono::sys_seconds>> last_modified;
    std::pair<std::optional<Status>, std::optional<std::uint64_t>> content_length;

private:
    constexpr static std::optional<Status> ret_second_if_first_is_unknown(const std::optional<Status>& first, const std::optional<Status>& second);
};
