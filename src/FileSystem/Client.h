#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QScopedPointer>
#include <QString>

class Client {
public:
    using ReplyHandler = std::function<void(QByteArray&&)>;

    Client(ReplyHandler&& handler) noexcept;

    void set_server_info(const QString& addr, const uint16_t port) noexcept;
    void request_file_list(const QString& path);
    void stop();

private:
    constexpr static char _file_list_request[] = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                                 "<D:propfind xmlns:D=\"DAV:\">"
                                                     "<D:prop>"
                                                         "<D:getlastmodified/>"
                                                         "<D:resourcetype/>"
                                                     "</D:prop>"
                                                 "</D:propfind>";

    const ReplyHandler _handler;
    QString _addr;
    uint16_t _port;
    QNetworkAccessManager _network_access_mgr;
    std::unique_ptr<QNetworkReply, QScopedPointerDeleteLater> _reply;
};
