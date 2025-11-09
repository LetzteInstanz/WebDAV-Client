#include "Client.h"

const std::unordered_map<char, std::string> Client::_reserved_chars{{'!', "%21"}, {'#', "%23"}, {'$', "%24"}, {'&', "%26"}, {'\'', "%27"}, {'(', "%28"}, {')', "%29"}, {'*', "%2a"}, {'+', "%2b"}, {',', "%2c"}, {':', "%3a"}, {';', "%3b"}, {'=', "%3d"}, {'?', "%3f"}, {'@', "%40"}, {'[', "%5b"}, {']', "%5d"}};

Client::Client(QStringView addr, std::uint16_t port) : _addr(addr.toString()), _port(port) {
    qInfo(qUtf8Printable(QObject::tr("Connecting to %s:%d…")), qUtf8Printable(_addr), _port);
}

void Client::request(RequestId&& id, const std::filesystem::path& path, RequestOptions properties, bool recursive, Handlers&& handlers, bool replace) {
    assert(handlers.first);
    assert(handlers.second);
    const std::string encoded_path = percent_encode(path);
    qDebug(qUtf8Printable(QObject::tr("Client: encoded path: %s")), encoded_path.c_str());
    const QString url = "http://" + _addr + ':' + QString::number(_port) + QString::fromStdString(encoded_path); // todo: set username and password
    QNetworkRequest req(url);
    req.setRawHeader("Depth", recursive ? "infinity" : "1");
    const QByteArray data = create_request(properties);
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
    auto finish = [this, id, reply = reply.get(), finish_handler = std::move(handlers.second)]() {
        assert(reply->bytesAvailable() == 0);
        finish_handler(id.second, reply->error());
        const auto it = std::ranges::find(_replies.find(id), std::ranges::end(_replies), reply, [](const auto& pair){ return pair.second.get(); });
        _replies.erase(it);
    };
    QObject::connect(reply.get(), &QNetworkReply::finished, std::move(finish));
    if (replace)
        _replies.erase(id);

    _replies.emplace(std::move(id), std::move(reply));
}

void Client::abort(const RequestId& id) {
    if (_replies.erase(id) > 0)
        qDebug(qUtf8Printable(QObject::tr("The request with ID %d is aborted")), id.second);
}

std::string Client::percent_encode(const std::filesystem::path& path) {
    const std::string original = path.generic_string();
    std::ostringstream stream;
    const auto end = std::ranges::end(_reserved_chars);
    for (const char ch : original) {
        const auto it = _reserved_chars.find(ch);
        if (it == end)
            stream << ch;
        else
            stream << it->second;
    }
    return stream.str();
}

QByteArray Client::create_request(RequestOptions properties) {
    QByteArray request("<?xml version=\"1.0\" encoding=\"utf-8\"?><D:propfind xmlns:D=\"DAV:\"><D:prop>");
    if (to_bool(properties & RequestOptions::ResourceType))
        request.append("<D:resourcetype/>");

    if (to_bool(properties & RequestOptions::CreationDate))
        request.append("<D:creationdate/>");

    if (to_bool(properties & RequestOptions::GetLastModified))
        request.append("<D:getlastmodified/>");

    if (to_bool(properties & RequestOptions::GetContentLength))
        request.append("<D:getcontentlength/>");

    request.append("</D:prop></D:propfind>");
    return request;
}
