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
    QFile _file;
    QJsonObject _obj;
};
