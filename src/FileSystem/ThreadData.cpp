#include "ThreadData.h"

#include "../QtUtil.h"
#include "Client.h"
#include "Parser/Parser.h"

namespace {
    constexpr Client::RequestOptions to_request_options(FileSystemModel::DataSet set) {
        switch (set) {
            case FileSystemModel::DataSet::Basic:
                return Client::RequestOptions::ResourceType | Client::RequestOptions::GetContentLength;

            case FileSystemModel::DataSet::Full:
                return Client::RequestOptions::All;

            default:
                assert(false);
                break;
        }
        return Client::RequestOptions::All;
    }
}

FileSystemModel::ThreadData::ThreadData(QString&& addr, std::uint16_t port) : _client(std::make_unique<::Client>(std::move(addr), port)) {}

FileSystemModel::ThreadData::~ThreadData() { _client->abort_all(); }

const std::unordered_set<QNetworkReply::NetworkError> FileSystemModel::ThreadData::_content_errors{ // note: RemoteHostClosedError happens on Apache (at least 2.4.65) sometimes, if the requested resource isn't accessible
    QNetworkReply::ContentAccessDenied, QNetworkReply::RemoteHostClosedError, QNetworkReply::ContentNotFoundError, QNetworkReply::ContentOperationNotPermittedError, QNetworkReply::ContentGoneError
};

void FileSystemModel::ThreadData::request_data(Id id, RequestTuple&& request) {
    auto request_func = [this, id, request = std::move(request)]() mutable {
        const auto recursive = std::get<IsRecursive>(request);
        const auto set = std::get<DataSet>(request);
        RecursionInfo info = recursive ? std::optional(std::pair(set, false)) : std::nullopt;
        ParseStateTuple tuple{std::move(info), Parsers(), std::move(std::get<ReadyFunc>(request)), std::move(std::get<ErrorFunc>(request))};
#ifdef NDEBUG
        const auto state_it = _parse_state_by_fs_id_map.emplace(id, std::move(tuple)).first;
#else
        const auto [state_it, emplaced] = _parse_state_by_fs_id_map.emplace(id, std::move(tuple));
        assert(emplaced);
#endif
        auto& parsers = std::get<Parsers>(state_it->second);
        request_data(id, parsers, std::get<std::filesystem::path>(request), set, recursive);
    };
    invoke_method(loop, std::move(request_func));
}

void FileSystemModel::ThreadData::stop() { invoke_method(loop, [this]() { loop.exit(0); }); }

void FileSystemModel::ThreadData::abort_request(std::vector<Id>&& ids, std::function<void (std::vector<Id>&&)>&& notif_func) {
    auto abort = [this, ids = std::move(ids), notif_func = std::move(notif_func)]() mutable {
        const auto subrange = std::ranges::remove_if(ids, [this](auto id) { return !this->abort(id); });
        ids.erase(std::ranges::cbegin(subrange), std::ranges::cend(subrange));
        notif_func(std::move(ids));
    };
    invoke_method(loop, std::move(abort));
}

void FileSystemModel::ThreadData::abort_request_sync(std::vector<Id>&& ids) { invoke_method_sync(loop, [this, ids = std::move(ids)]() { std::ranges::for_each(ids, [this](Id id) { abort(id); }); }); }

void FileSystemModel::ThreadData::request_data(Id id, Parsers& parsers, const std::filesystem::path& path, DataSet set, bool recursive) {
    auto parser = std::make_unique<Parser>(path);
    auto handlers = std::make_pair(std::bind(&Parser::parse_response_portion, parser.get(), std::placeholders::_1), std::bind(&FileSystemModel::ThreadData::finish, this, std::placeholders::_1, std::placeholders::_2, std::reference_wrapper(*parser)));
    _client->request(id, path, to_request_options(set), recursive, std::move(handlers));
    parsers.emplace(std::ranges::lower_bound(parsers, parser), std::move(parser));
}

void FileSystemModel::ThreadData::notify_about_finish(const ReadyFunc& func, Id id) {
    const auto it = _fs_by_id_map.find(id);
    assert(it != std::ranges::end(_fs_by_id_map));
    qDebug(qUtf8Printable(QObject::tr("FileSystemModel: request completed (ID: %d)")), id);
    if (func)
        func(std::move(*it));

    _fs_by_id_map.erase(it);
}

void FileSystemModel::ThreadData::call_error_func(const ErrorFunc func, Id id, bool has_parse_error, QNetworkReply::NetworkError qt_error, bool is_critical) {
    if (func)
        func(id, has_parse_error ? Error::ResponseParseError : Error::NetworkError, qt_error, is_critical);
}

void FileSystemModel::ThreadData::finish(Id id, QNetworkReply::NetworkError error, Parser& parser) {
    const auto state_it = _parse_state_by_fs_id_map.find(id);
    assert(state_it != std::ranges::cend(_parse_state_by_fs_id_map));
    auto& [recur_info, parsers, _, _] = state_it->second;
    const auto parser_it = std::ranges::lower_bound(parsers, &parser, {}, [](const auto& unique_ptr) { return unique_ptr.get(); });
    const std::unique_ptr<Parser> parser_ptr = std::move(*parser_it);
    parsers.erase(parser_it);
    const auto is_recursion = recur_info.has_value();
    const auto is_self_made_recursion = is_recursion && recur_info->second;
    const auto obj_pair_it = _fs_by_id_map.find(id);
    const auto result_exists = obj_pair_it != std::ranges::cend(_fs_by_id_map);
    if (const auto has_network_error = error != QNetworkReply::NoError; has_network_error || parser_ptr->has_error()) { // note: Parser::has_error() method should only be called, if there is no network error, because at least QNetworkReply::RemoteHostClosedError results in a parse error; the method should only be called once, because it also writes the reply to the log
        if (const auto add_self_made_recursion = is_recursion && !is_self_made_recursion && (error == QNetworkReply::ContentAccessDenied || error == QNetworkReply::RemoteHostClosedError)) { // note: RemoteHostClosedError happens on Apache (at least 2.4.65) during a request with infinity-depth, if some subresource isn't accessible
            qDebug(qUtf8Printable(QObject::tr("FileSystemModel: request with infinity-depth header failed (ID: %d)")), id);
            recur_info->second = true;
            request_data(id, parsers, parser_ptr->get_current_path(), recur_info->first, false);
            return;
        }
        if (is_self_made_recursion && result_exists && !_content_errors.contains(error)) {
            _fs_by_id_map.erase(obj_pair_it);
            _client->abort(id);
            call_error_func(std::get<ErrorFunc>(state_it->second), id, !has_network_error, error, true);
            _parse_state_by_fs_id_map.erase(state_it);
            return;
        }
        if (const auto& root_request_failed = !result_exists; root_request_failed || !is_self_made_recursion) {
            call_error_func(std::get<ErrorFunc>(state_it->second), id, !has_network_error, error, !_content_errors.contains(error));
            if (parsers.empty())
                _parse_state_by_fs_id_map.erase(state_it);

            return;
        }
        if (has_network_error)
            qWarning(qUtf8Printable(QObject::tr("An error occurred during a subrequest: %s (ID: %d)")), qUtf8Printable(to_qstring(error)), id);

        if (!parsers.empty())
            return;

        notify_about_finish(std::get<ReadyFunc>(state_it->second), id);
        _parse_state_by_fs_id_map.erase(state_it);
        return;
    }
    auto [new_curr_dir, new_objects] = parser_ptr->get_result();
    if (is_self_made_recursion) {
        auto view = new_objects | std::ranges::views::filter([](const FileSystemObject& object) { return object.get_type() == FileSystemObject::Type::Directory; });
        for (const FileSystemObject& object : view) {
            qDebug(qUtf8Printable(QObject::tr("FileSystemModel: subrequest (path: %s, ID: %d)")), qUtf8Printable(QString::fromStdString(object.get_path().generic_string())), id);
            request_data(id, parsers, object.get_path(), recur_info->first, false);
        }
    }
    if (!is_self_made_recursion || !result_exists) {
        _fs_by_id_map.emplace(id, std::make_pair(std::move(*new_curr_dir), std::move(new_objects)));
    } else {
        auto& [_, objects] = obj_pair_it->second;
        std::ranges::move(new_objects, std::back_inserter(objects));
    }
    if (!parsers.empty())
        return;

    notify_about_finish(std::get<ReadyFunc>(state_it->second), id);
    _parse_state_by_fs_id_map.erase(state_it);
}

bool FileSystemModel::ThreadData::abort(Id id) {
    const auto parse_state_it = _parse_state_by_fs_id_map.find(id);
    if (parse_state_it == std::ranges::cend(_parse_state_by_fs_id_map))
        return false;

    _client->abort(id);
    _parse_state_by_fs_id_map.erase(parse_state_it);
    qDebug(qUtf8Printable(QObject::tr("FileSystemModel: request aborted (ID: %d)")), id);
    return true;
};
