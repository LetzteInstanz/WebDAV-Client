#pragma once

#include "../Util.h"

class Client {
public:
    using DataHandler = std::function<void (const ReadBuffer&)>;
    using FinishHandler = std::function<void (QNetworkReply::NetworkError)>;

    Client(QStringView addr, std::uint16_t port, DataHandler&& data_handler, FinishHandler&& finish_handler);

    void request_file_list(const std::filesystem::path& path);
    void abort();

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
    const DataHandler _data_handler;
    const FinishHandler _finish_handler;
    QNetworkAccessManager _network_access_mgr;
    std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater> _reply;
    ReadBuffer _buffer;
};
