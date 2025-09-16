#pragma once

#include "../Util.h"

class Client {
public:
    using DataHandler = std::function<void (const ReadBuffer&)>;
    using FinishHandler = std::function<void (QNetworkReply::NetworkError)>;
    using Handlers = std::pair<DataHandler, FinishHandler>;
    enum class PropfindProperty { ResourceType = 1, CreationDate = 1 << 1, GetLastModified = 1 << 2, GetContentLength = 1 << 3, All = ResourceType | CreationDate | GetLastModified | GetContentLength };

    Client(QStringView addr, std::uint16_t port);

    std::uint32_t request(const std::filesystem::path& path, PropfindProperty properties, bool recursive, Handlers&& handlers);
    void abort(std::uint32_t id);

private:
    static QByteArray create_request(PropfindProperty properties);

private:
    QString _addr;
    std::uint16_t _port;
    QNetworkAccessManager _network_access_mgr;
    std::uint32_t _next_id = 0;
    std::unordered_map<std::uint32_t, std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater>> _replies;
    ReadBuffer _buffer;
};

constexpr Client::PropfindProperty operator|(Client::PropfindProperty lhs, Client::PropfindProperty rhs);
constexpr Client::PropfindProperty operator&(Client::PropfindProperty lhs, Client::PropfindProperty rhs);
