#include "FileSystemModel.h"

#include "Client.h"
#include "Parser.h"

FileSystemModel::FileSystemModel()
    : _client(std::make_unique<Client>(std::bind(&FileSystemModel::handle_reply, this, std::placeholders::_1))), _parser(std::make_unique<Parser>())
{}

FileSystemModel::~FileSystemModel() = default;

void FileSystemModel::set_server_info(const QString& addr, const uint16_t port, const QString& path) {
    _client->set_server_info(addr, port);
    _current_dir = path;
}

void FileSystemModel::request_file_list() { _client->request_file_list(QStringLiteral("/")); }

void FileSystemModel::stop() { _client->stop(); }

void FileSystemModel::handle_reply(QByteArray&& data) {
    try {
        Parser::CurrDirResultPair pair = _parser->parse_propfind_reply(_current_dir, data);
        _objects = std::move(pair.second);
    } catch (const std::runtime_error& e) {
        qCritical(qUtf8Printable(QObject::tr("An error has occured during reply parse: %s. The reply text: \n%s")), qUtf8Printable(QObject::tr(e.what())), qUtf8Printable(data));
        // todo: emit error signal to Qml::FileSystemModel
    }
}
