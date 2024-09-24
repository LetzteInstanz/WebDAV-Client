#pragma once

#include <chrono>
#include <cstdint>
#include <utility>

#include <QString>

class FileSystemObject {
public:
    enum class Status {None, Unknown, Ok, Unauthorized, Forbidden, NotFound};
    enum class Type {Directory, File};

    FileSystemObject(QString&& name, Type type, std::pair<Status, std::chrono::sys_seconds>&& creation_time, std::pair<Status, std::chrono::sys_seconds>&& modification_time, std::pair<Status, std::uint64_t>&& size) noexcept;

    QString get_name() const noexcept { return _name; }
    Type get_type() const noexcept { return _type; }
    bool is_creation_time_valid() const noexcept { return _creation_time.first == Status::Ok; }
    std::chrono::sys_seconds get_creation_time() const { return _creation_time.second; }
    bool is_modification_time_valid() const noexcept { return _modification_time.first == Status::Ok; }
    std::chrono::sys_seconds get_modification_time() const { return _modification_time.second; }
    bool is_size_valid() const noexcept { return _size.first == Status::Ok; }
    std::uint64_t get_size() const noexcept { return _size.second; }

private:
    QString _name;
    Type _type;
    std::pair<Status, std::chrono::sys_seconds> _creation_time;
    std::pair<Status, std::chrono::sys_seconds> _modification_time;
    std::pair<Status, std::uint64_t> _size;
};
