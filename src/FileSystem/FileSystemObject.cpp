#include "FileSystemObject.h"

FileSystemObject::FileSystemObject(QString&& name, Type type, std::pair<Status, std::chrono::sys_seconds>&& creation_time, std::pair<Status, std::chrono::sys_seconds>&& modification_time, std::pair<Status, std::uint64_t>&& size) noexcept
    : _name(std::move(name)), _type(type), _creation_time(std::move(creation_time)), _modification_time(std::move(modification_time)), _size(std::move(size))
{}
