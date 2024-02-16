#include "FileSystemObject.h"

FileSystemObject::FileSystemObject(QString&& name, Type type, std::pair<Status, std::tm>&& modification_time) noexcept
    : _name(std::move(name)), _type(type), _modification_time(std::move(modification_time))
{}
