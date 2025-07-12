#include "JsonFile.h"

JsonFile::JsonFile(std::string_view filename) : _path(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation).toStdString()) {
#ifndef ANDROID
    _path /= "WebDAVClient";
#endif
    qDebug(qUtf8Printable(QObject::tr("Settings file path: %s")), _path.c_str());
    try {
        std::filesystem::create_directory(_path);
    } catch (const std::filesystem::filesystem_error& e) {
        qCritical(qUtf8Printable(QObject::tr("%s: could not create directory %s: %s")), "JsonFile", _path.c_str(), e.what());
        return;
    }
    _path /= filename;
    _file.open(_path, std::ios_base::in);
    if (_file.is_open()) {
        try {
            _json_data = nlohmann::json::parse(_file);
        } catch (const nlohmann::json::parse_error& e) {
            std::error_code error;
            const std::uintmax_t size = std::filesystem::file_size(_path, error);
            if (error)
                qCritical(qUtf8Printable(QObject::tr("%s: could not get file size (%s): %s")), "JsonFile", _path.filename().c_str(), error.message().c_str());

            if (size != 0)
                qWarning(qUtf8Printable(QObject::tr("%s: invalid JSON format: %s")), "JsonFile", e.what());
        }
        _file.close();
    }
    if (!_json_data)
        set_root_object(nlohmann::json::object());
}

JsonFile::~JsonFile() = default;

nlohmann::json JsonFile::get_root_object() const { return _json_data ? *_json_data : nlohmann::json::object(); }

void JsonFile::set_root_object(nlohmann::json&& json) {
    assert(!_json_data || _json_data != json);
    _json_data = std::move(json);
    _file.open(_path, std::ios_base::out | std::ios_base::trunc); // note: it is necessary to open file for writing each time to overwrite it
    if (!_file.is_open()) {
        qCritical(qUtf8Printable(QObject::tr("%s: could not create file %s")), "JsonFile", _path.filename().c_str());
        return;
    }
    try {
        _file << std::setw(4) << *_json_data << std::endl;
    }
    catch (const nlohmann::json::type_error& e) {
        qCritical(qUtf8Printable(QObject::tr("%s: could not write to file %s: %s")), "JsonFile", _path.filename().c_str(), e.what());
    }
    catch (const std::ios_base::failure& e) {
        qCritical(qUtf8Printable(QObject::tr("%s: could not write to file %s: %s")), "JsonFile", _path.filename().c_str(), e.what());
    }
    _file.close();
}
