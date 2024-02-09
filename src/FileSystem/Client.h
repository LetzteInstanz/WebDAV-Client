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
    using ReplyHandler = std::function<void(QByteArray&&)>;
    using ErrorHandler = std::function<void(QNetworkReply::NetworkError)>;

    Client(ReplyHandler&& reply_handler, ErrorHandler&& error_handler) noexcept;

    void set_server_info(const QStringView& addr, uint16_t port) noexcept;
    void request_file_list(const QStringView& path);
    void stop();

private:
    constexpr static char _file_list_request[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                                 "<D:propfind xmlns:D=\"DAV:\">"
                                                     "<D:prop>"
                                                         "<D:getlastmodified/>"
                                                         "<D:resourcetype/>"
                                                     "</D:prop>"
                                                 "</D:propfind>";

    const ReplyHandler _reply_handler;
    const ErrorHandler _error_handler;
    QString _addr;
    uint16_t _port;
    QNetworkAccessManager _network_access_mgr;
    std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater> _reply;
};
