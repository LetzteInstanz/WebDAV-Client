#include "FileSystemModel.h"

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
    }
}

const std::unordered_set<QNetworkReply::NetworkError> FileSystemModel::_content_errors{QNetworkReply::ContentAccessDenied, QNetworkReply::ContentNotFoundError, QNetworkReply::ContentOperationNotPermittedError, QNetworkReply::ContentGoneError};

FileSystemModel::FileSystemModel(std::shared_ptr<Client> client, State mode) : _client(std::move(client)), _mode(mode) {
    qDebug(qUtf8Printable(QObject::tr("FileSystemModel: created: mode: %s")), _mode == State::Single ? "Single" : "Multiple");
}

FileSystemModel::~FileSystemModel() {
    std::ranges::for_each(_request_state_by_filesystem_id_map, [this](const auto& pair) { _client->abort(Client::RequestId(this, pair.first)); });
    qDebug().noquote().nospace() << QObject::tr("FileSystemModel: destroyed");
}

std::uint32_t FileSystemModel::request_data(const std::filesystem::path& path, DataSet set, bool recursive) {
    const auto file_system_it = std::ranges::cbegin(_file_system_by_id_map);
    const auto exists = _mode == State::Single && file_system_it != std::ranges::cend(_file_system_by_id_map);
    const auto curr_path = exists ? file_system_it->second.first.get_path() : std::filesystem::path("/");
    const auto new_path = (curr_path / path / std::filesystem::path()).lexically_normal();
    const auto id = _next_id;
    _next_id = _mode == State::Multiple ? ++_next_id : _next_id;
    const auto state_it = _request_state_by_filesystem_id_map.insert_or_assign(id, std::pair(recursive ? std::optional(std::pair(set, false)) : std::nullopt, Parsers())).first;
    Parsers& parsers = state_it->second.second;
    request_data(id, parsers, new_path, set, recursive);
    qDebug(qUtf8Printable(QObject::tr("FileSystemModel: the resource %s is requested: ID: %d")), qUtf8Printable(QString::fromStdString(new_path.generic_string())), id);
    return id;
}

void FileSystemModel::abort_request(std::uint32_t file_system_id) {
    const auto parse_state_it = _request_state_by_filesystem_id_map.find(file_system_id);
    if (parse_state_it == std::ranges::cend(_request_state_by_filesystem_id_map))
        return;

    _client->abort(Client::RequestId(this, file_system_id));
    _request_state_by_filesystem_id_map.erase(parse_state_it);
    qDebug(qUtf8Printable(QObject::tr("FileSystemModel: the request with ID %d is aborted")), file_system_id);
}

void FileSystemModel::abort_all_requests() {
    if (_request_state_by_filesystem_id_map.empty())
        return;

    QDebug debug = qDebug().noquote().nospace();
    debug << QObject::tr("FileSystemModel: all requests are aborted: IDs: ");
    for (auto begin = std::ranges::begin(_request_state_by_filesystem_id_map), it = begin; it != std::ranges::end(_request_state_by_filesystem_id_map); ++it) {
        _client->abort(Client::RequestId(this, it->first));
        if (it != begin)
            debug << ", ";

        debug << it->first;
    }
    _request_state_by_filesystem_id_map.clear();
}

void FileSystemModel::add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func) { _notify_func_by_obj_map.emplace(obj, std::move(func)); }

void FileSystemModel::remove_notification_func(const void* obj) {
    const auto it = _notify_func_by_obj_map.find(obj);
    if (it != std::ranges::cend(_notify_func_by_obj_map))
        _notify_func_by_obj_map.erase(it);
}

void FileSystemModel::set_error_func(NotifyAboutErrorFunc&& func) { _error_func = std::move(func); }

FileSystemObject FileSystemModel::get_curr_dir_object(std::uint32_t file_system_id) const { return get_pair(file_system_id).first; }

FileSystemObject FileSystemModel::get_object(std::uint32_t file_system_id, std::size_t index) const { return get_pair(file_system_id).second[index]; }

std::size_t FileSystemModel::get_count(std::uint32_t file_system_id) const noexcept { return get_pair(file_system_id).second.size(); }

const FileSystemModel::ObjectPair& FileSystemModel::get_pair(std::uint32_t file_system_id) const noexcept {
    const auto it = _file_system_by_id_map.find(file_system_id);
    assert(it != std::ranges::end(_file_system_by_id_map));
    return it->second;
}

void FileSystemModel::remove(std::uint32_t file_system_id) {
    assert(_file_system_by_id_map.contains(file_system_id));
    _file_system_by_id_map.erase(_file_system_by_id_map.find(file_system_id));
    qDebug(qUtf8Printable(QObject::tr("FileSystemModel: file system with ID %d is removed")), file_system_id);
}

void FileSystemModel::request_data(std::uint32_t file_system_id, Parsers& parsers, const std::filesystem::path& path, DataSet set, bool recursive) {
    auto parser = std::make_unique<Parser>(path);
    auto handlers = std::make_pair(std::bind(&Parser::parse_response_portion, parser.get(), std::placeholders::_1), std::bind(&FileSystemModel::finish, this, std::placeholders::_1, std::placeholders::_2, std::reference_wrapper(*parser)));
    _client->request(Client::RequestId(this, file_system_id), path, to_request_options(set), recursive, std::move(handlers), _mode == State::Single);
    qDebug(qUtf8Printable(QObject::tr("FileSystemModel: file system is requested: path: %s, ID: %d")), qUtf8Printable(QString::fromStdString(path.generic_string())), file_system_id);
    parsers.emplace(std::ranges::lower_bound(parsers, parser), std::move(parser));
}

void FileSystemModel::notify_about_finish(std::uint32_t file_system_id, bool is_finished) {
    if (!is_finished)
        return;

    qDebug(qUtf8Printable(QObject::tr("FileSystemModel: the request with ID %d is finished")), file_system_id);
    std::ranges::for_each(_notify_func_by_obj_map, [file_system_id](const std::pair<const void*, const NotifyAboutUpdateFunc>& pair) { pair.second(file_system_id); });
}

void FileSystemModel::call_error_func(std::uint32_t file_system_id, bool has_parse_error, QNetworkReply::NetworkError qt_error, bool is_critical) {
    _error_func(file_system_id, has_parse_error ? Error::ResponseParseError : Error::NetworkError, qt_error, is_critical);
}

void FileSystemModel::finish(std::uint32_t file_system_id, QNetworkReply::NetworkError error, Parser& parser) {
    const auto remove_empty_parser_container = [this](const Parsers& parsers, auto state_it) {
        if (parsers.empty()) {
            _request_state_by_filesystem_id_map.erase(state_it);
            return true;
        }
        return false;
    };

    const auto state_it = _request_state_by_filesystem_id_map.find(file_system_id);
    assert(state_it != std::ranges::end(_request_state_by_filesystem_id_map));
    auto& [option_pair, parsers] = state_it->second;
    const auto parser_it = std::ranges::lower_bound(parsers, &parser, {}, [](const auto& unique_ptr) { return unique_ptr.get(); });
    const std::unique_ptr<Parser> parser_ptr = std::move(*parser_it);
    parsers.erase(parser_it);
    const auto is_recursion = option_pair.has_value();
    const auto is_self_made_recursion = is_recursion && option_pair->second;
    const auto obj_pair_it = _file_system_by_id_map.find(file_system_id);
    const auto result_exists = obj_pair_it != std::ranges::end(_file_system_by_id_map);
    const bool has_parse_error = parser_ptr->has_error();
    if (has_parse_error || error != QNetworkReply::NoError) {
        if (const auto add_self_made_recursion = is_recursion && !is_self_made_recursion && (error == QNetworkReply::ContentAccessDenied || error == QNetworkReply::RemoteHostClosedError)) { // note: RemoteHostClosedError happens on Apache (at least 2.4.65) during a request with infinity-depth, if some subresource isn't accessible
            qDebug(qUtf8Printable(QObject::tr("FileSystemModel: the request with the infinity-depth header and ID %d failed")), file_system_id);
            option_pair->second = true;
            request_data(file_system_id, parsers, parser_ptr->get_current_path(), option_pair->first, false);
            return;
        }
        if (is_self_made_recursion && result_exists && !_content_errors.contains(error)) {
            _file_system_by_id_map.erase(obj_pair_it);
            _request_state_by_filesystem_id_map.erase(state_it);
            _client->abort(Client::RequestId(this, file_system_id));
            call_error_func(file_system_id, has_parse_error, error, true);
            return;
        }
        const auto& root_request_failed = !result_exists;
        if (!is_self_made_recursion || root_request_failed) {
            remove_empty_parser_container(parsers, state_it);
            call_error_func(file_system_id, has_parse_error, error, !_content_errors.contains(error));
            return;
        }
        if (!has_parse_error)
            qWarning(qUtf8Printable(QObject::tr("A network error occurred during a subrequest: %s")), qUtf8Printable(to_qstring(error)));

        notify_about_finish(file_system_id, remove_empty_parser_container(parsers, state_it));
        return;
    }
    auto [new_curr_dir, new_objects] = parser_ptr->get_result();
    if (is_self_made_recursion) {
        auto view = new_objects | std::ranges::views::filter([](const FileSystemObject& object){ return object.get_type() == FileSystemObject::Type::Directory; });
        std::ranges::for_each(view, [this, file_system_id, &parsers, &option_pair](const FileSystemObject& object){ request_data(file_system_id, parsers, object.get_path(), option_pair->first, false); });
    }
    if (!is_self_made_recursion || !result_exists) {
        _file_system_by_id_map.insert_or_assign(file_system_id, std::make_pair(std::move(*new_curr_dir), std::move(new_objects)));
    } else {
        auto& [_, objects] = obj_pair_it->second;
        std::ranges::move(new_objects, std::back_inserter(objects));
    }
    notify_about_finish(file_system_id, remove_empty_parser_container(parsers, state_it));
}
