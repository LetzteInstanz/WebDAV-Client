#include "Settings.h"

Settings::Settings() : _appDirName("WebDAVClient_2212ca02-1a86-4707-b731-492959a8fd40") {
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    if (!dir.exists(_appDirName)) {
        if (!dir.mkdir(_appDirName, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner))
            ;// todo: throw exception

    }
    dir.cd(_appDirName);
    _file.setFileName(dir.absolutePath() + "/config.json");
    if (!_file.open(QIODeviceBase::ReadWrite, QFileDevice::ReadOwner | QFileDevice::WriteOwner)) {
        //const QString err_str(QObject::tr("Could not create config file: "));
        //throw std::runtime_error(err_str.toStdString() + _file.symLinkTarget().toStdString())*/;
        int n = 0;
    }
    const QByteArray raw_data = _file.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(raw_data);
    const QJsonObject obj = doc.object();
    const QJsonValue value = obj["download_path"];
    _download_path = value.isString() ? value.toString() : QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
}
