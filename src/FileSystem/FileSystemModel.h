#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>
#include <functional>
#include <memory>

#include <QByteArray>
#include <QNetworkReply>
#include <QString>
#include <QStringView>

#ifdef ANDROID
#include "FileSystemObject.h" // note: Building under Android fails with forward declaration
#else
class FileSystemObject;
#endif

class Client;
class Parser;

class FileSystemModel {
public:
    enum class Error {ReplyParseError, NetworkError, UncorrectPath};

    using NotifyAboutUpdateFunc = std::function<void ()>;
    using NotifyAboutErrorFunc = std::function<void (Error, QNetworkReply::NetworkError)>;

    FileSystemModel();
    ~FileSystemModel();

    bool is_cur_dir_root_path() const noexcept;
    QString get_current_path() const noexcept;
    void set_server_info(QStringView addr, std::uint16_t port);
    void set_root_path(QStringView absolute_path);
    void request_file_list(QStringView relative_path);
    void abort_request();
    void disconnect();
    void add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func) noexcept;
    void remove_notification_func(const void* obj);
    void set_error_func(NotifyAboutErrorFunc&& func) noexcept;
    FileSystemObject get_curr_dir_object() const noexcept;
    FileSystemObject get_object(std::size_t index) const noexcept;
    std::size_t size() const noexcept;

private:
    static QString&& add_slash_to_start(QString&& path);
    static QString&& add_slash_to_end(QString&& path);
    static QString handle_double_dots(QStringView path);
    void handle_reply(QByteArray&& data);
    void handle_error(QNetworkReply::NetworkError error);

private:
    std::unique_ptr<Client> _client;
    QString _root_path;
    std::unordered_map<const void*, const NotifyAboutUpdateFunc> _notify_func_by_obj_map;
    NotifyAboutErrorFunc _error_func;
    QString _prev_path;
    QString _current_path;
    std::unique_ptr<FileSystemObject> _curr_dir_obj;
    std::deque<FileSystemObject> _objects;
};
