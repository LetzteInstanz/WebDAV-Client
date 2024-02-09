#pragma once

#include <ctime>
#include <utility>

#include <QString>

class FileSystemObject {
public:
    enum class Status { None, Unknown, Ok, Unauthorized, Forbidden, NotFound };
    enum class Type { Directory, File };

    FileSystemObject(QString&& name, const Type type, std::pair<Status, std::tm>&& last_modified_pair) noexcept;

    QString get_name() const noexcept { return _name; }
    Type get_type() const noexcept { return _type; }
    bool is_last_modified_valid() const noexcept { return _last_modified_pair.first == Status::Ok; }
    std::tm get_last_modified() const noexcept { return _last_modified_pair.second; }

private:
    QString _name;
    Type _type;
    std::pair<Status, std::tm> _last_modified_pair;
};
