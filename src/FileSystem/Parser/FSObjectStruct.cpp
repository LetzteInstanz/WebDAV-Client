#include "FSObjectStruct.h"

std::optional<FSObjectStruct::Status> FSObjectStruct::to_status(QStringView str) {
    for (const std::pair<QString, Status>& pair : _str_code_pairs) {
        if (str.indexOf(pair.first) != -1)
            return pair.second;
    }
    return {};
}

void FSObjectStruct::replace_unknown_status(const std::optional<Status>& s) {
    type.first = ret_second_if_first_is_unknown(type.first, s);
    creation_date.first = ret_second_if_first_is_unknown(creation_date.first, s);
    last_modified.first = ret_second_if_first_is_unknown(last_modified.first, s);
    content_length.first = ret_second_if_first_is_unknown(content_length.first, s);
}

constexpr std::optional<FSObjectStruct::Status> FSObjectStruct::ret_second_if_first_is_unknown(const std::optional<Status>& first, const std::optional<Status>& second) {
    return first == FSObjectStruct::Status::Unknown ? second : first;
}

const std::vector<std::pair<QString, FSObjectStruct::Status>> FSObjectStruct::_str_code_pairs{{"200", Status::Ok},
                                                                                              {"401", Status::Unauthorized},
                                                                                              {"403", Status::Forbidden},
                                                                                              {"404", Status::NotFound}};
