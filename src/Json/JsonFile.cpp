#include "JsonFile.h"

const char* const JsonFile::_app_dir_name = "WebDAVClient_2212ca02-1a86-4707-b731-492959a8fd40";

JsonFile::JsonFile(const QString& filename) {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    if (!dir.exists(_app_dir_name)) {
        if (!dir.mkdir(_app_dir_name, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner))
            qCritical(qUtf8Printable(QObject::tr("Could not create directory \"%s/%s\"")), qUtf8Printable(dir.absolutePath()), _app_dir_name);
    }
    dir.cd(_app_dir_name);
    _file.setFileName(dir.absolutePath() + '/' + filename);
    if (!_file.open(QIODeviceBase::ReadWrite, QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
        qCritical(qUtf8Printable(QObject::tr("Could not create file \"%s\"")), qUtf8Printable(_file.fileName()));
        return;
    }
    const auto doc = QJsonDocument::fromJson(_file.readAll());
    _file.seek(0);
    if (doc.isObject())
        _obj = doc.object();
}

JsonFile::~JsonFile() {
    if (!_file.isOpen())
        return;

    _file.seek(0);
    _file.resize(_file.write(QJsonDocument(_obj).toJson()));
}
