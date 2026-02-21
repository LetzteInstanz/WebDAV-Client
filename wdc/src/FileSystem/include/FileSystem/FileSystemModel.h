#pragma once

#include <cstdint>
#include <deque>
#include <filesystem>
#include <functional>
#include <future>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QNetworkReply>
#include <QStringView>

#include "FileSystemObject.h"

class FileSystemModel final {
public:
    enum class Error {ResponseParseError, NetworkError};
    enum class DataSet {Basic, Full};
    enum class State {Single, Multiple};

    using ObjectPair = std::pair<FileSystemObject, std::deque<FileSystemObject>>;
    using Id = std::uint32_t;
    using FsPair = std::pair<Id, ObjectPair>;
    using ReadyFunc = std::function<void (FsPair&&)>;
    using IsCritical = bool;
    using ErrorFunc = std::function<void (Id, Error, QNetworkReply::NetworkError, IsCritical)>;
    using IdVector = std::vector<Id>;
    using IsRecursive = bool;
    using RequestTuple = std::tuple<std::filesystem::path, DataSet, IsRecursive, ReadyFunc, ErrorFunc>;

    FileSystemModel(QStringView addr, std::uint16_t port);
    ~FileSystemModel();

    [[nodiscard]] Id request_data(RequestTuple&& request);
    void abort_request(IdVector&& ids, std::function<void (IdVector&&)>&& notif_func);
    void abort_request_sync(IdVector&& ids);

private:
    struct ThreadData;

private:
    Id _next_id = 0;
    ThreadData* _thread_data = nullptr;
    std::future<int> _future;
    std::unordered_map<Id, ObjectPair> _file_system_by_id_map;
};
