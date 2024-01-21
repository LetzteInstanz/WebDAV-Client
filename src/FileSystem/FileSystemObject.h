#pragma once

#include <ctime>
#include <utility>

#include <QString>

class FileSystemObject {
public:
    enum class Status { None, Unknown, Ok, Unauthorized, Forbidden, NotFound };
    enum class Type { Directory, File };

    FileSystemObject(QString&& name, const Type type, std::pair<Status, std::tm>&& last_modified_pair);

    QString get_name() const { return _name; }
    void set_name(const QString& name) { _name = name; }
    Type get_type() const { return _type; }
    void set_type(Type type) { _type = type; }

    bool is_last_modified_valid() const { return _last_modified_pair.first == Status::Ok; }
    //void set_last_modified(const std::tm& time) { _last_modified_pair.second = time; }

    std::tm get_last_modified() const { return _last_modified_pair.second; }
    void set_last_modified(const std::tm& time) { _last_modified_pair.second = time; }

private:
    QString _name;
    Type _type;
    std::pair<Status, std::tm> _last_modified_pair;
};
