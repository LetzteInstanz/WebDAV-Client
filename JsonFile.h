#pragma once

#include <QFile>
#include <QJsonObject>
#include <QString>

class JsonFile {
public:
    JsonFile(const QString& filename);
    virtual ~JsonFile() = default;

protected:
    QJsonObject get_root_obj() const { return _obj; }
    void write(const QJsonObject& root_obj);

private:
    static const char* const _app_dir_name;

    QFile _file;
    QJsonObject _obj;
};
