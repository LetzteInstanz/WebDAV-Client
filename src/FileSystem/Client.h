#pragma once

#include "../Util.h"

class Client {
public:
    enum class RequestOptions { ResourceType = 1, CreationDate = 1 << 1, GetLastModified = 1 << 2, GetContentLength = 1 << 3, All = ResourceType | CreationDate | GetLastModified | GetContentLength };

    using RequestId = std::pair<const void*, std::uint32_t>;
    using DataHandler = std::function<void (const ReadBuffer&)>;
    using FinishHandler = std::function<void (std::uint32_t, QNetworkReply::NetworkError)>;
    using Handlers = std::pair<DataHandler, FinishHandler>;

    Client(QStringView addr, std::uint16_t port);

    void request(RequestId&& id, const std::filesystem::path& path, RequestOptions properties, bool recursive, Handlers&& handlers, bool replace);
    void abort(const RequestId& id);

private:
    static std::string percent_encode(const std::filesystem::path& path);
    static QByteArray create_request(RequestOptions properties);

private:
    static const std::unordered_map<char, std::string> _reserved_chars;
    QString _addr;
    std::uint16_t _port;
    QNetworkAccessManager _network_access_mgr;

    struct IdHash {
        std::size_t operator()(const RequestId& id) const noexcept {
            const auto ptr_hash = std::hash<const void*>()(id.first);
            const auto id_hash = std::hash<std::uint32_t>()(id.second);
            return ptr_hash ^ id_hash + 0x517cc1b727220a95 + (ptr_hash << 6) + (ptr_hash >> 2);
        }
    };
    std::unordered_multimap<RequestId, std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater>, IdHash> _replies;
    ReadBuffer _buffer;
};

constexpr Client::RequestOptions operator|(Client::RequestOptions lhs, Client::RequestOptions rhs) { return to_type<Client::RequestOptions>(to_uint(lhs) | to_uint(rhs)); }
constexpr Client::RequestOptions operator&(Client::RequestOptions lhs, Client::RequestOptions rhs) { return to_type<Client::RequestOptions>(to_uint(lhs) & to_uint(rhs)); }
