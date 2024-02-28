#pragma once

#include <chrono>
#include <cstdint>
#include <utility>
#include <vector>

#include <QString>
#include <QStringView>

#include "../FileSystemObject.h"

struct FSObjectStruct {
    using Status = FileSystemObject::Status;
    using Type = FileSystemObject::Type;

    static QString extract_name(const QStringView& abs_path);
    static Status to_status(const QStringView& str);

    void replace_unknown_status(Status s);

    bool is_curr_dir_obj = false;
    QString name;
    std::pair<Status, Type> type = {Status::None, Type::File};
    std::pair<Status, std::chrono::sys_seconds> creation_date = {Status::None, {}};
    std::pair<Status, std::chrono::sys_seconds> last_modified = {Status::None, {}};
    std::pair<Status, uint64_t> content_length = {Status::None, 0};

private:
    constexpr static Status ret_second_if_first_is_unknown(Status first, Status second);

private:
    static const std::vector<std::pair<QString, Status>> _str_code_pairs;
};
