#pragma once

#include <QFile>
#include <QJsonObject>
#include <QString>

class JsonFile {
public:
    JsonFile(const QString& filename);
    virtual ~JsonFile();

protected:
    QJsonObject get_root_obj() const { return _obj; }
    void set_root_obj(const QJsonObject& obj) { _obj = obj; }

private:
    static const char* const _app_dir_name;

    QFile _file;
    QJsonObject _obj;
};
