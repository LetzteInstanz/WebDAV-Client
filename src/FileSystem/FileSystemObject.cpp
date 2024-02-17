#include "FileSystemObject.h"

FileSystemObject::FileSystemObject(QString&& name, Type type, std::pair<Status, time_t>&& creation_time, std::pair<Status, time_t>&& modification_time) noexcept
    : _name(std::move(name)), _type(type), _creation_time(std::move(creation_time)), _modification_time(std::move(modification_time))
{}
