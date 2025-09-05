#include "FileSystemModel.h"

#include "Client.h"
#include "Parser/Parser.h"

FileSystemModel::FileSystemModel(QStringView addr, std::uint16_t port, const std::filesystem::path& root_path)
    : _parser(std::make_unique<Parser>()),
      _client(std::make_unique<Client>(addr, port, std::bind(&Parser::parse_response_portion, _parser.get(), std::placeholders::_1), std::bind(&FileSystemModel::finish, this, std::placeholders::_1))),
      _root_path((std::filesystem::path("/") / root_path / std::filesystem::path()).lexically_normal()), _current_path(_root_path)
{
    qDebug().noquote() << QObject::tr("The file system model is being created");
}

FileSystemModel::~FileSystemModel() { qDebug().noquote() << QObject::tr("The file system model is being destroyed"); }

bool FileSystemModel::is_cur_dir_root_path() const { return _root_path == get_current_path(); }

std::filesystem::path FileSystemModel::get_current_path() const { return _current_path; }

void FileSystemModel::request_file_list(const std::filesystem::path& path) {
    auto new_path = (_current_path / path / std::filesystem::path()).lexically_normal();
    _client->request_file_list(new_path);
    _parser->set_current_path(std::move(new_path));
}

void FileSystemModel::abort_request() {
    _client->abort();
    _parser->reset();
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

void FileSystemModel::finish(QNetworkReply::NetworkError error) {
    const auto handle_error = [this](Error custom_error, QNetworkReply::NetworkError network_error) {
        _parser->reset();
        if (_error_func)
            _error_func(custom_error, network_error);
    };

    if (error != QNetworkReply::NoError) {
        handle_error(Error::NetworkError, error);
        return;
    }
    try {
        Parser::Result result = _parser->get_result();
        _current_path = _parser->get_current_path();
        _parser->reset();
        _curr_dir_obj = std::move(result.first);
        _objects = std::move(result.second);
        std::for_each(std::cbegin(_notify_func_by_obj_map), std::cend(_notify_func_by_obj_map), [](const auto& pair) { pair.second(); });
    } catch (const Parser::Exception&) {
        handle_error(Error::ResponseParseError, QNetworkReply::NoError);
    }
}
