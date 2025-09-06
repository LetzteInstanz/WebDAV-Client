#pragma once

#include "../Util.h"

class Client {
public:
    using DataHandler = std::function<void (const ReadBuffer&)>;
    using FinishHandler = std::function<void (QNetworkReply::NetworkError)>;
    using Handlers = std::pair<DataHandler, FinishHandler>;

    Client(QStringView addr, std::uint16_t port);

    std::uint32_t request_file_list(Handlers&& handlers, const std::filesystem::path& path, bool recursive);
    void abort(std::uint32_t id);

private:
    constexpr static char _file_list_request[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                                 "<D:propfind xmlns:D=\"DAV:\">"
                                                     "<D:prop>"
                                                         "<D:creationdate/>"
                                                         "<D:getlastmodified/>"
                                                         "<D:resourcetype/>"
                                                         "<D:getcontentlength/>"
                                                     "</D:prop>"
                                                 "</D:propfind>";

    QString _addr;
    std::uint16_t _port;
    QNetworkAccessManager _network_access_mgr;
    std::uint32_t _next_id = 0;
    std::unordered_map<std::uint32_t, std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater>> _replies;
    ReadBuffer _buffer;
};
