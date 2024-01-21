#include "FileSystemObject.h"

FileSystemObject::FileSystemObject(QString&& name, const Type type, std::pair<Status, std::tm>&& last_modified_pair)
    : _name(std::move(name)), _type(type), _last_modified_pair(std::move(last_modified_pair))
{}
