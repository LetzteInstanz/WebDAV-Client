#include "Client.h"

Client::Client(ReplyHandler&& reply_handler, ErrorHandler&& error_handler) noexcept
    : _reply_handler(std::move(reply_handler)), _error_handler(std::move(error_handler)) {}

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
    QObject::connect(_reply.get(), &QNetworkReply::errorOccurred, [this](QNetworkReply::NetworkError e) { _error_handler(e); });
    QObject::connect(_reply.get(), &QIODevice::readyRead, [this]() { _reply_handler(_reply->readAll()); });
}

void Client::stop() {
    if (_reply != nullptr)
        _reply->abort();
}
