#include "Client.h"

Client::Client(ReplyHandler&& reply_handler, ErrorHandler&& error_handler) noexcept
    : _reply_handler(std::move(reply_handler)), _error_handler(std::move(error_handler)) {}

void Client::set_server_info(const QStringView& addr, uint16_t port) noexcept {
    _addr = addr.toString();
    _port = port;
}

void Client::request_file_list(const QStringView& path) {
    QNetworkRequest req;
    const QString url = "http://" + _addr + ':' + QString::number(_port) + path.toString();
    req.setUrl(QUrl(url)); // todo: set username and password
    qInfo(qUtf8Printable(QObject::tr("The request is occurring: %s")), qUtf8Printable(url));
    req.setRawHeader("Depth", "1");
    const QByteArray data = _file_list_request;
    req.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    _reply.reset(_network_access_mgr.sendCustomRequest(req, "PROPFIND", data));
    const auto read = [this]() {
        const QNetworkReply::NetworkError error = _reply->error();
        if (error == QNetworkReply::NoError)
            _reply_handler(_reply->readAll());
        else
            _error_handler(error);
    };
    QObject::connect(_reply.get(), &QNetworkReply::finished, read);
}

void Client::abort() {
    if (!_reply)
        return;

    qDebug(qUtf8Printable(QObject::tr("The request is being aborted")));
    _reply->abort();
    _reply.reset();
}
