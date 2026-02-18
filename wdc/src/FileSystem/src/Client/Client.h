#pragma once

#include "FileSystemModel.h"
#include "../Util.h"

class Client final {
public:
    enum class RequestOptions { ResourceType = 1, CreationDate = 1 << 1, GetLastModified = 1 << 2, GetContentLength = 1 << 3, All = ResourceType | CreationDate | GetLastModified | GetContentLength };

    using DataHandler = std::function<void (const ReadBuffer&)>;
    using FinishHandler = std::function<void (FileSystemModel::Id, QNetworkReply::NetworkError)>;
    using Handlers = std::pair<DataHandler, FinishHandler>;

    Client(QStringView addr, std::uint16_t port);

    void request(FileSystemModel::Id id, const std::filesystem::path& path, RequestOptions properties, bool recursive, Handlers&& handlers);
    void abort(FileSystemModel::Id id);
    void abort_all();

private:
    using ReplyMultimap = std::unordered_multimap<FileSystemModel::Id, std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater>>;

    void abort(const std::pair<ReplyMultimap::const_iterator, ReplyMultimap::const_iterator>& range);

private:
    QString _addr;
    std::uint16_t _port;
    QNetworkAccessManager _network_access_mgr;

    ReplyMultimap _replies;
    ReadBuffer _buffer;
};

constexpr Client::RequestOptions operator|(Client::RequestOptions lhs, Client::RequestOptions rhs) { return to_type<Client::RequestOptions>(to_uint(lhs) | to_uint(rhs)); }
constexpr Client::RequestOptions operator&(Client::RequestOptions lhs, Client::RequestOptions rhs) { return to_type<Client::RequestOptions>(to_uint(lhs) & to_uint(rhs)); }
