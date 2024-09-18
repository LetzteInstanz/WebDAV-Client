#include "FSObjectStruct.h"

#include "../../pch.h"

QString FSObjectStruct::extract_name(const QStringView& abs_path) {
    if (abs_path.empty())
        return QString();

    if (abs_path == QStringLiteral("/"))
        return abs_path.toString();

    const qsizetype from = abs_path.back() == '/' ? -2 : -1;
    const qsizetype pos = abs_path.lastIndexOf('/', from);
    assert(pos != -1);
    return QStringView(std::begin(abs_path) + pos + 1, std::end(abs_path) + from + 1).toString();
}

FSObjectStruct::Status FSObjectStruct::to_status(const QStringView& str) {
    for (const std::pair<QString, Status>& pair : _str_code_pairs) {
        if (str.indexOf(pair.first) != -1)
            return pair.second;
    }
    return Status::None;
}

void FSObjectStruct::replace_unknown_status(Status s) {
    type.first = ret_second_if_first_is_unknown(type.first, s);
    creation_date.first = ret_second_if_first_is_unknown(creation_date.first, s);
    last_modified.first = ret_second_if_first_is_unknown(last_modified.first, s);
    content_length.first = ret_second_if_first_is_unknown(content_length.first, s);
}

constexpr FSObjectStruct::Status FSObjectStruct::ret_second_if_first_is_unknown(Status first, Status second) {
    return first == FSObjectStruct::Status::Unknown ? second : first;
}

const std::vector<std::pair<QString, FSObjectStruct::Status>> FSObjectStruct::_str_code_pairs{{"200", Status::Ok},
                                                                                              {"401", Status::Unauthorized},
                                                                                              {"403", Status::Forbidden},
                                                                                              {"404", Status::NotFound}};
