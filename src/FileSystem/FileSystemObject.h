#pragma once

#include <ctime>
#include <utility>

#include <QString>

class FileSystemObject {
public:
    enum class Status {None, Unknown, Ok, Unauthorized, Forbidden, NotFound};
    enum class Type {Directory, File};

    FileSystemObject(QString&& name, Type type, std::pair<Status, std::tm>&& modification_time) noexcept;

    QString get_name() const noexcept { return _name; }
    Type get_type() const noexcept { return _type; }
    bool is_modification_time_valid() const noexcept { return _modification_time.first == Status::Ok; }
    std::tm get_modification_time() const noexcept { return _modification_time.second; }

private:
    QString _name;
    Type _type;
    std::pair<Status, std::tm> _modification_time;
};
