#include "Client.h"

Client::Client(QStringView addr, std::uint16_t port, DataHandler&& data_handler, FinishHandler&& finish_handler)
    : _addr(addr.toString()), _port(port), _data_handler(std::move(data_handler)), _finish_handler(std::move(finish_handler)) {}

void Client::request_file_list(const std::filesystem::path& path) {
    QNetworkRequest req;
    const QString url = "http://" + _addr + ':' + QString::number(_port) + QString::fromStdString(path.generic_string());
    req.setUrl(QUrl(url)); // todo: set username and password
    qInfo(qUtf8Printable(QObject::tr("The request is occurring: %s")), qUtf8Printable(url));
    req.setRawHeader("Depth", "1");
    const QByteArray data = _file_list_request;
    req.setHeader(QNetworkRequest::ContentLengthHeader, data.size());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    _reply.reset(_network_access_mgr.sendCustomRequest(req, "PROPFIND", data));
    auto read = [this]() {
        qint64 n;
        while ((n = _reply->read(_buffer.data(), _buffer.size() - 1)) > 0) {
            _buffer[n] = 0;
            _data_handler(_buffer);
        }
    };
    QObject::connect(_reply.get(), &QIODevice::readyRead, std::move(read));
    QObject::connect(_reply.get(), &QNetworkReply::finished, [this]() { assert(_reply->bytesAvailable() == 0); _finish_handler(_reply->error()); });
}

void Client::abort() {
    if (!_reply || _reply->isFinished())
        return;

    qDebug().noquote() << QObject::tr("The request is being aborted");
    _reply->abort();
    _reply.reset();
}
