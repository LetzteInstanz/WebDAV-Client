#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QString>
#include <QStringView>

class Client {
public:
    using ReplyHandler = std::function<void (QByteArray&&)>;
    using ErrorHandler = std::function<void (QNetworkReply::NetworkError)>;

    Client(ReplyHandler&& reply_handler, ErrorHandler&& error_handler) noexcept;

    void set_server_info(QStringView addr, std::uint16_t port) noexcept;
    void request_file_list(QStringView path);
    void abort();

private:
    constexpr static char _file_list_request[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                                                 "<D:propfind xmlns:D=\"DAV:\">\n"
                                                     "<D:prop>\n"
                                                         "<D:creationdate/>\n"
                                                         "<D:getlastmodified/>\n"
                                                         "<D:resourcetype/>\n"
                                                         "<D:getcontentlength/>\n"
                                                     "</D:prop>\n"
                                                 "</D:propfind>";

    const ReplyHandler _reply_handler;
    const ErrorHandler _error_handler;
    QString _addr;
    std::uint16_t _port;
    QNetworkAccessManager _network_access_mgr;
    std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater> _reply;
};
