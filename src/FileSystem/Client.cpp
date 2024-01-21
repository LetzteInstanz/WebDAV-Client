#include "Client.h"

Client::Client(ReplyHandler&& handler) noexcept : _handler(std::move(handler)) {}

void Client::set_server_info(const QString& addr, const uint16_t port) noexcept {
    _addr = addr;
    _port = port;
}

void Client::request_file_list(const QString& path) {
    QNetworkRequest req;
    req.setUrl(QUrl("http://" + _addr + ':' + QString::number(_port) + '/' + path)); // todo: set username and password
    req.setRawHeader("Depth", "1");
    const QByteArray data = _file_list_request;
    req.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    _reply.reset(_network_access_mgr.sendCustomRequest(req, "PROPFIND", data));
    QObject::connect(_reply.get(), &QNetworkReply::finished, [this](){ _handler(_reply->readAll()); });
}

void Client::stop() {
    if (_reply != nullptr)
        _reply->abort();
}
