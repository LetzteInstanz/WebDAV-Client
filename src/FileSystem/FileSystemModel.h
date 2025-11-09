#pragma once

#include "FileSystemObject.h"

class Client;
class Parser;

class FileSystemModel final {
public:
    enum class Error {ResponseParseError, NetworkError};
    enum class DataSet {Basic, Full};
    enum class State {Single, Multiple};

    using NotifyAboutUpdateFunc = std::function<void (std::uint32_t)>;
    using IsCritical = bool;
    using NotifyAboutErrorFunc = std::function<void (std::uint32_t, Error, QNetworkReply::NetworkError, IsCritical)>;

    FileSystemModel(std::shared_ptr<Client> client, State mode);
    ~FileSystemModel();

    std::uint32_t request_data(const std::filesystem::path& path, DataSet set, bool recursive);
    void abort_request(std::uint32_t file_system_id);
    void abort_all_requests();
    void add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func);
    void remove_notification_func(const void* obj);
    void set_error_func(NotifyAboutErrorFunc&& func);
    FileSystemObject get_curr_dir_object(std::uint32_t file_system_id) const;
    FileSystemObject get_object(std::uint32_t file_system_id, std::size_t index) const;
    std::size_t get_count(std::uint32_t file_system_id) const noexcept;
    void remove(std::uint32_t file_system_id);

private:
    using ObjectPair = std::pair<FileSystemObject, std::deque<FileSystemObject>>;
    using Parsers = std::deque<std::unique_ptr<Parser>>;

    const ObjectPair& get_pair(std::uint32_t file_system_id) const noexcept;
    void request_data(std::uint32_t file_system_id, Parsers& parsers, const std::filesystem::path& path, DataSet set, bool recursive);
    void notify_about_finish(std::uint32_t file_system_id, bool is_finished);
    void call_error_func(std::uint32_t file_system_id, bool has_parse_error, QNetworkReply::NetworkError qt_error, bool is_critical);
    void finish(std::uint32_t file_system_id, QNetworkReply::NetworkError error, Parser& parser);

private:
    using SelfMadeRecursion = bool;
    using RequestOptionPair = std::optional<std::pair<DataSet, SelfMadeRecursion>>;

    static const std::unordered_set<QNetworkReply::NetworkError> _content_errors;
    std::shared_ptr<Client> _client;
    std::uint32_t _next_id = 0;
    const State _mode;
    std::unordered_map<const void*, const NotifyAboutUpdateFunc> _notify_func_by_obj_map;
    NotifyAboutErrorFunc _error_func;
    std::unordered_map<std::uint32_t, ObjectPair> _file_system_by_id_map;
    std::unordered_map<std::uint32_t, std::pair<RequestOptionPair, Parsers>> _request_state_by_filesystem_id_map;
};
