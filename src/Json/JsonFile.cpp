#include "JsonFile.h"

#include "../pch.h"

JsonFile::JsonFile(const QString& filename) {
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
#ifndef Q_OS_ANDROID
    path += "/WebDAVClient_2212ca02-1a86-4707-b731-492959a8fd40";
#endif
    QDir dir;
    qDebug(qUtf8Printable(QObject::tr("Settings path: \"%s\"")), qUtf8Printable(path));
    if (!dir.mkpath(path)) {
        qCritical(qUtf8Printable(QObject::tr("Could not create directory \"%s\"")), qUtf8Printable(path));
        return;
    }
    _file.setFileName(path + '/' + filename);
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
