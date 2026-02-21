#include "FSObjectStruct.h"

#include <Common/Util.h>

std::optional<FSObjectStruct::Status> FSObjectStruct::to_status(QStringView str) {
    auto from = str.indexOf(' ');
    if (from < 0)
        return {};

    const auto to = str.indexOf(' ', ++from);
    if (to <= 0)
        return {};

    str = QStringView(std::begin(str) + from, std::begin(str) + to);
    bool ok;
    const auto code = to_type<Status>(str.toUShort(&ok));
    if (!ok)
        return {};

    return code;
}

void FSObjectStruct::replace_unknown_status(const std::optional<Status>& s) {
    type.first = ret_second_if_first_is_unknown(type.first, s);
    creation_date.first = ret_second_if_first_is_unknown(creation_date.first, s);
    last_modified.first = ret_second_if_first_is_unknown(last_modified.first, s);
    content_length.first = ret_second_if_first_is_unknown(content_length.first, s);
}

constexpr std::optional<FSObjectStruct::Status> FSObjectStruct::ret_second_if_first_is_unknown(const std::optional<Status>& first, const std::optional<Status>& second) {
    return first == Status::Unknown ? second : first;
}
