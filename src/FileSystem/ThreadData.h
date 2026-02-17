#pragma once

#include "FileSystemModel.h"

class Client;
class Parser;

struct FileSystemModel::ThreadData {
    ThreadData(QString&& addr, std::uint16_t port);
    ~ThreadData();

    QEventLoop loop;

    void request_data(Id id, RequestTuple&& request);
    void stop();
    void abort_request(std::vector<Id>&& ids, std::function<void (std::vector<Id>&&)>&& notif_func);
    void abort_request_sync(std::vector<Id>&& ids);

private:
    using Parsers = std::deque<std::unique_ptr<Parser>>;

    void request_data(Id id, Parsers& parsers, const std::filesystem::path& path, DataSet set, bool recursive);
    void notify_about_finish(const ReadyFunc& func, Id id);
    void call_error_func(const ErrorFunc func, Id id, bool has_parse_error, QNetworkReply::NetworkError qt_error, bool is_critical);
    void finish(Id id, QNetworkReply::NetworkError error, Parser& parser);
    bool abort(Id id);

private:
    using IsSelfMadeRecursion = bool;
    using RecursionInfo = std::optional<std::pair<DataSet, IsSelfMadeRecursion>>;
    using ParseStateTuple = std::tuple<RecursionInfo, Parsers, ReadyFunc, ErrorFunc>;

    static const std::unordered_set<QNetworkReply::NetworkError> _content_errors;

    std::unique_ptr<Client> _client;
    std::unordered_map<Id, ParseStateTuple> _parse_state_by_fs_id_map;
    std::unordered_map<Id, ObjectPair> _fs_by_id_map;
};
