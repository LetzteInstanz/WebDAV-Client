#include "FileSystemModel.h"

#include "../Util.h"
#include "Client.h"
#ifndef ANDROID
#include "FileSystemObject.h"
#endif
#include "Parser/Parser.h"

FileSystemModel::FileSystemModel()
    : _client(std::make_unique<Client>(std::bind(&FileSystemModel::handle_reply, this, std::placeholders::_1),
                                       std::bind(&FileSystemModel::handle_error, this, std::placeholders::_1)))
{
}

FileSystemModel::~FileSystemModel() = default;

bool FileSystemModel::is_cur_dir_root_path() const noexcept { return _root_path == get_current_path(); }

QString FileSystemModel::get_current_path() const noexcept { return _current_path; }

void FileSystemModel::set_server_info(QStringView addr, std::uint16_t port) { _client->set_server_info(addr, port); }

void FileSystemModel::set_root_path(QStringView absolute_path) {
    _root_path = add_slash_to_end(add_slash_to_start(absolute_path.toString()));
    _current_path = _root_path;
    _prev_path.clear();
}

void FileSystemModel::request_file_list(QStringView relative_path) {
    _prev_path = _current_path;
    _current_path = process_two_dots_in_path(_current_path + add_slash_to_end(relative_path.toString()));
    _client->request_file_list(_current_path);
}

void FileSystemModel::abort_request() { _client->abort(); }

void FileSystemModel::disconnect() {
    abort_request();
    qDebug().noquote() << QObject::tr("The file system model is being reset");
    _objects.clear();
    _curr_dir_obj.reset();
    _prev_path.clear();
    _current_path.clear();
}

void FileSystemModel::add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func) noexcept { _notify_func_by_obj_map.emplace(obj, std::move(func)); }

void FileSystemModel::remove_notification_func(const void* obj) {
    const auto it = _notify_func_by_obj_map.find(obj);
    if (it != std::cend(_notify_func_by_obj_map))
        _notify_func_by_obj_map.erase(it);
}

void FileSystemModel::set_error_func(NotifyAboutErrorFunc&& func) noexcept { _error_func = std::move(func); }

FileSystemObject FileSystemModel::get_curr_dir_object() const noexcept {
    assert(_curr_dir_obj);
    return *_curr_dir_obj;
}

FileSystemObject FileSystemModel::get_object(std::size_t index) const noexcept { return _objects[index]; }

std::size_t FileSystemModel::size() const noexcept { return _objects.size(); }

QString&& FileSystemModel::add_slash_to_start(QString&& path) {
    if (path.isEmpty() || path.front() != '/')
        path = '/' + path;

    return std::move(path);
}

QString&& FileSystemModel::add_slash_to_end(QString&& path) {
    if (!path.isEmpty() && path.back() != '/')
        path += '/';

    return std::move(path);
}

void FileSystemModel::handle_reply(QByteArray&& data) {
    try {
        Parser::Result result = Parser::parse_propfind_reply(_current_path, data);
        _curr_dir_obj = std::move(result.first);
        _objects = std::move(result.second);
        std::for_each(std::cbegin(_notify_func_by_obj_map), std::cend(_notify_func_by_obj_map), [](const auto& pair) { pair.second(); });
    } catch (const std::runtime_error& e) {
        _current_path = _prev_path;
        qCritical(qUtf8Printable(QObject::tr("An error has occured during reply parse: %s. The reply text: \n%s")), qUtf8Printable(QObject::tr(e.what())), qUtf8Printable(data));
        if (_error_func)
            _error_func(Error::ReplyParseError, QNetworkReply::NetworkError::NoError);
    }
}

void FileSystemModel::handle_error(QNetworkReply::NetworkError error) {
    _current_path = _prev_path;
    if (_error_func)
        _error_func(Error::NetworkError, error);
}
