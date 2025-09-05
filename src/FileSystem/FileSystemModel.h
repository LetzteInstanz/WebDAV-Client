#pragma once

#include "FileSystemObject.h"

class Client;
class Parser;

class FileSystemModel {
public:
    enum class Error {ResponseParseError, NetworkError};

    using NotifyAboutUpdateFunc = std::function<void ()>;
    using NotifyAboutErrorFunc = std::function<void (Error, QNetworkReply::NetworkError)>;

    FileSystemModel(QStringView addr, std::uint16_t port, const std::filesystem::path& root_path);
    ~FileSystemModel();

    bool is_cur_dir_root_path() const;
    std::filesystem::path get_current_path() const;
    void request_file_list(const std::filesystem::path& path);
    void abort_request();
    void add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func);
    void remove_notification_func(const void* obj);
    void set_error_func(NotifyAboutErrorFunc&& func) noexcept;
    FileSystemObject get_curr_dir_object() const;
    FileSystemObject get_object(std::size_t index) const;
    std::size_t get_size() const noexcept;

private:
    void finish(QNetworkReply::NetworkError error);

private:
    std::unique_ptr<Parser> _parser;
    std::unique_ptr<Client> _client;
    std::unordered_map<const void*, const NotifyAboutUpdateFunc> _notify_func_by_obj_map;
    NotifyAboutErrorFunc _error_func;
    std::filesystem::path _root_path;
    std::filesystem::path _current_path;
    std::optional<FileSystemObject> _curr_dir_obj;
    std::deque<FileSystemObject> _objects;
};
