#include "Client.h"

Client::Client(QStringView addr, std::uint16_t port) : _addr(addr.toString()), _port(port) {}

std::uint32_t Client::request_file_list(Handlers&& handlers, const std::filesystem::path& path, bool recursive) {
    assert(handlers.first);
    assert(handlers.second);
    const QString url = "http://" + _addr + ':' + QString::number(_port) + QString::fromStdString(path.generic_string()); // todo: set username and password
    QNetworkRequest req(url);
    qInfo(qUtf8Printable(QObject::tr("The request is occurring: %s")), qUtf8Printable(url));
    req.setRawHeader("Depth", recursive ? "infinity" : "1");
    const QByteArray data = _file_list_request;
    req.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    auto reply = std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater>(_network_access_mgr.sendCustomRequest(req, "PROPFIND", data));
    auto read = [this, reply = reply.get(), data_handler = std::move(handlers.first)]() {
        qint64 n;
        while ((n = reply->read(_buffer.data(), _buffer.size() - 1)) > 0) {
            _buffer[n] = 0;
            data_handler(_buffer);
        }
    };
    QObject::connect(reply.get(), &QIODevice::readyRead, std::move(read));
    _next_id++;
    auto finish = [this, id = _next_id, reply = reply.get(), finish_handler = std::move(handlers.second)]() {
        assert(reply->bytesAvailable() == 0);
        finish_handler(reply->error());
        _replies.erase(_replies.find(id));
    };
    QObject::connect(reply.get(), &QNetworkReply::finished, std::move(finish));
    _replies.emplace(_next_id, std::move(reply));
    return _next_id;
}

void Client::abort(std::uint32_t id) {
    const auto reply_it = _replies.find(id);
    if (reply_it == std::cend(_replies))
        return;

    std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater>& reply = reply_it->second;
    if (reply->isFinished())
        return;

    qDebug().noquote() << QObject::tr("The request is being aborted");
    reply->abort();
    _replies.erase(reply_it);
}
