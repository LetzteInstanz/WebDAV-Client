#include "FileSystemModel.h"

#include "Client.h"
#include "Parser.h"

FileSystemModel::FileSystemModel()
    : _client(std::make_unique<Client>(std::bind(&FileSystemModel::handle_reply, this, std::placeholders::_1),
                                       std::bind(&FileSystemModel::handle_error, this, std::placeholders::_1))), _parser(std::make_unique<Parser>())
{}

FileSystemModel::~FileSystemModel() = default;

void FileSystemModel::set_server_info(const QString& addr, const uint16_t port, const QString& path) {
    QString corrected = path;
    if (corrected.isEmpty() || corrected.front() != '/')
        corrected = '/' + corrected;

    if (corrected.back() != '/')
        corrected += '/';

    _client->set_server_info(addr, port);
    _current_dir = corrected;
}

void FileSystemModel::request_file_list() { _client->request_file_list(QStringLiteral("/")); }

void FileSystemModel::stop() { _client->stop(); }

void FileSystemModel::add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func) noexcept { _notify_func_by_obj_map.emplace(obj, std::move(func)); }

void FileSystemModel::remove_notification_func(const void* obj) {
    const auto it = _notify_func_by_obj_map.find(obj);
    if (it != _notify_func_by_obj_map.end())
        _notify_func_by_obj_map.erase(it);
}

void FileSystemModel::set_error_func(NotifyAboutErrorFunc&& func) noexcept { _error_func = std::move(func); }

void FileSystemModel::handle_reply(QByteArray&& data) {
    try {
        Parser::CurrDirResultPair pair = _parser->parse_propfind_reply(_current_dir, data);
        _objects = std::move(pair.second);
        std::for_each(std::begin(_notify_func_by_obj_map), std::end(_notify_func_by_obj_map), [](const auto& pair) { pair.second(); });
    } catch (const std::runtime_error& e) {
        qCritical(qUtf8Printable(QObject::tr("An error has occured during reply parse: %s. The reply text: \n%s")), qUtf8Printable(QObject::tr(e.what())), qUtf8Printable(data));
        if (_error_func)
            _error_func(Error::ReplyParseError, QNetworkReply::NetworkError::NoError);
    }
}

void FileSystemModel::handle_error(QNetworkReply::NetworkError error) {
    if (_error_func)
        _error_func(Error::NetworkError, error);
}
