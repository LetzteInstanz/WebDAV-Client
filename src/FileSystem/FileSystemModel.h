#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>

#include <QByteArray>
#include <QNetworkReply>
#include <QString>

#include "FileSystemObject.h"

class Client;
class Parser;

class FileSystemModel {
public:
    enum class Error {ReplyParseError, NetworkError};

    using NotifyAboutUpdateFunc = std::function<void()>;
    using NotifyAboutErrorFunc = std::function<void(Error, QNetworkReply::NetworkError)>;

    FileSystemModel();
    ~FileSystemModel();

    void set_server_info(const QString& addr, const uint16_t port, const QString& path);
    void request_file_list();
    void stop();
    void add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func) noexcept;
    void remove_notification_func(const void* obj);
    void set_error_func(NotifyAboutErrorFunc&& func) noexcept;
    FileSystemObject get_object(const size_t index) const noexcept { return _objects[index]; }
    size_t size() const noexcept { return _objects.size(); }

private:
    void handle_reply(QByteArray&& data);
    void handle_error(QNetworkReply::NetworkError error);

private:
    std::unique_ptr<Client> _client;
    std::unique_ptr<Parser> _parser;
    QString _current_dir;
    std::unordered_map<const void*, const NotifyAboutUpdateFunc> _notify_func_by_obj_map;
    NotifyAboutErrorFunc _error_func;
    std::deque<FileSystemObject> _objects;
};
