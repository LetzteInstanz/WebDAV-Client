#include "FileSystemModel.h"

#include "Client.h"
#include "Parser/Parser.h"

FileSystemModel::FileSystemModel(QStringView addr, std::uint16_t port, const std::filesystem::path& root_path)
    : _client(std::make_unique<Client>(addr, port, std::bind(&FileSystemModel::handle_reply, this, std::placeholders::_1), std::bind(&FileSystemModel::handle_error, this, std::placeholders::_1))),
      _root_path((std::filesystem::path("/") / root_path / std::filesystem::path()).lexically_normal()), _current_path(_root_path)
{
    qDebug().noquote() << QObject::tr("The file system model is being created");
}

FileSystemModel::~FileSystemModel() { qDebug().noquote() << QObject::tr("The file system model is being destroyed"); }

bool FileSystemModel::is_cur_dir_root_path() const { return _root_path == get_current_path(); }

std::filesystem::path FileSystemModel::get_current_path() const { return _current_path; }

void FileSystemModel::request_file_list(const std::filesystem::path& path) {
    _prev_path = _current_path;
    _current_path /= path / std::filesystem::path();
    _current_path = _current_path.lexically_normal();
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

void FileSystemModel::add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func) { _notify_func_by_obj_map.emplace(obj, std::move(func)); }

void FileSystemModel::remove_notification_func(const void* obj) {
    const auto it = _notify_func_by_obj_map.find(obj);
    if (it != std::cend(_notify_func_by_obj_map))
        _notify_func_by_obj_map.erase(it);
}

void FileSystemModel::set_error_func(NotifyAboutErrorFunc&& func) noexcept { _error_func = std::move(func); }

FileSystemObject FileSystemModel::get_curr_dir_object() const {
    assert(_curr_dir_obj);
    return *_curr_dir_obj;
}

FileSystemObject FileSystemModel::get_object(std::size_t index) const { return _objects[index]; }

std::size_t FileSystemModel::get_size() const noexcept { return _objects.size(); }

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
