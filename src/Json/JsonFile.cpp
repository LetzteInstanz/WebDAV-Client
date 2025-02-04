#include "JsonFile.h"

JsonFile::JsonFile(std::string_view filename) {
    auto path_to_dir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation).toStdString();
#ifndef Q_OS_ANDROID
    path_to_dir += "/WebDAVClient_2212ca02-1a86-4707-b731-492959a8fd40";
#endif
    qDebug(qUtf8Printable(QObject::tr("Settings path: \"%s\"")), qUtf8Printable(path_to_dir.c_str()));
    try {
        std::filesystem::create_directory(path_to_dir);
    } catch (const std::filesystem::filesystem_error&) {
        qCritical(qUtf8Printable(QObject::tr("Could not create directory \"%s\"")), qUtf8Printable(path_to_dir.c_str()));
        return;
    }
    _path_to_file = path_to_dir + '/' + std::string(filename);
    _file.open(_path_to_file, std::ios_base::in);
    if (!_file.is_open())
        return;

    _json_data = nlohmann::json::parse(_file);
    _file.close();
}

JsonFile::~JsonFile() {
    _file.open(_path_to_file, std::ios_base::out | std::ios_base::trunc);
    if (!_file.is_open()) {
        qCritical(qUtf8Printable(QObject::tr("Could not create file \"%s\"")), qUtf8Printable(_path_to_file.c_str()));
        return;
    }
    _file << std::setw(4) << _json_data << std::endl;
    if (_file.fail())
        qCritical(qUtf8Printable(QObject::tr("Could not write to the file \"%s\"")), qUtf8Printable(_path_to_file.c_str()));
}

void JsonFile::set_root_obj(nlohmann::json&& json) { _json_data = std::move(json); }
