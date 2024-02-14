#include "FileSystemModel.h"

#include "Client.h"
#include "Parser.h"

FileSystemModel::FileSystemModel()
    : _client(std::make_unique<Client>(std::bind(&FileSystemModel::handle_reply, this, std::placeholders::_1),
                                       std::bind(&FileSystemModel::handle_error, this, std::placeholders::_1))), _parser(std::make_unique<Parser>())
{
#ifndef NDEBUG
    QString test = "/";
    assert(handle_double_dots(test) == "/");
    test = "/test/test2/../../test3/test4/";
    assert(handle_double_dots(test) == "/test3/test4/");
    test = "/test/../test2/../test3/";
    assert(handle_double_dots(test) == "/test3/");
    test = "/test/..g/";
    assert(handle_double_dots(test) == test);
    test = "/test/g../";
    assert(handle_double_dots(test) == test);
    test = "/test/..../";
    assert(handle_double_dots(test) == test);
#endif
}

FileSystemModel::~FileSystemModel() = default;

bool FileSystemModel::is_cur_dir_root_path() const noexcept {
    assert(_curr_dir_obj);
    QString abs_path = _parent_path + _curr_dir_obj->get_name();
    add_slash_to_end(abs_path);
    return _root_path == abs_path;
}

void FileSystemModel::set_server_info(const QStringView& addr, uint16_t port) { _client->set_server_info(addr, port); }

void FileSystemModel::set_root_path(const QStringView& absolute_path) {
    QString final_path = absolute_path.toString();
    add_slash_to_start(final_path);
    add_slash_to_end(final_path);
    _root_path = final_path;
}

void FileSystemModel::request_file_list(const QStringView& relative_path) {
    QString abs_path = relative_path.toString();
    if (_curr_dir_obj) {
        const QString name = _curr_dir_obj->get_name();
        if (name != QStringLiteral("/"))
            add_slash_to_start(abs_path);

        abs_path = _parent_path + name + abs_path;
    } else {
        abs_path = _root_path + abs_path;
    }
    add_slash_to_end(abs_path);
    _client->request_file_list(handle_double_dots(abs_path));
}

void FileSystemModel::abort_request() { _client->abort(); }

void FileSystemModel::disconnect() {
    abort_request();
    qDebug(qUtf8Printable(QObject::tr("The file system model is being reset")));
    _objects.clear();
    _curr_dir_obj.reset();
    _parent_path.clear();
}

void FileSystemModel::add_notification_func(const void* obj, NotifyAboutUpdateFunc&& func) noexcept { _notify_func_by_obj_map.emplace(obj, std::move(func)); }

void FileSystemModel::remove_notification_func(const void* obj) {
    const auto it = _notify_func_by_obj_map.find(obj);
    if (it != std::end(_notify_func_by_obj_map))
        _notify_func_by_obj_map.erase(it);
}

void FileSystemModel::set_error_func(NotifyAboutErrorFunc&& func) noexcept { _error_func = std::move(func); }

FileSystemObject FileSystemModel::get_curr_dir_object() const noexcept {
    assert(_curr_dir_obj);
    return *_curr_dir_obj;
}

FileSystemObject FileSystemModel::get_object(size_t index) const noexcept { return _objects[index]; }

size_t FileSystemModel::size() const noexcept { return _objects.size(); }

void FileSystemModel::add_slash_to_start(QString& path) {
    if (path.isEmpty() || path.front() != '/')
        path = '/' + path;
}

void FileSystemModel::add_slash_to_end(QString& path) {
    if (path.back() != '/')
        path += '/';
}

QString FileSystemModel::handle_double_dots(const QStringView& path) {
    assert(!path.empty());
    assert(path[0] == '/');
    assert(path.back() == '/');

    if (path.size() == 1)
        return path.toString();

    struct {
        bool after_slash = false;
        int dots = 0;
    } state;
    std::vector<QStringView> dirs;
    auto was_never_double_dots = true;
    for (auto from = std::begin(path), it = std::begin(path), end = std::end(path); it != end; ++it) {
        const QChar& ch = *it;
        if (ch == '/') {
            if (state.after_slash && state.dots == 2) {
                assert(!dirs.empty());
                dirs.pop_back();
                was_never_double_dots = false;
            } else {
                if (from != it)
                    dirs.emplace_back(QStringView(from, it));
            }
            state.after_slash = true;
            state.dots = 0;
            from = it;
        } else if (ch == '.') {
            ++state.dots;
        } else {
            state.after_slash = false;
        }
    }
    if (was_never_double_dots)
        return path.toString();

    QString ret;
    std::for_each(std::cbegin(dirs), std::cend(dirs), [&ret](const auto& dir) { ret += dir; });
    ret += '/';
    return ret;
}

void FileSystemModel::handle_reply(QByteArray&& data) {
    try {
        Parser::Result result = _parser->parse_propfind_reply(data);
        _parent_path = std::move(std::get<0>(result));
        _curr_dir_obj = std::move(std::get<1>(result));
        _objects = std::move(std::get<2>(result));
        std::for_each(std::begin(_notify_func_by_obj_map), std::end(_notify_func_by_obj_map), [](const auto& pair) { pair.second(); });
    } catch (const std::runtime_error& e) {
        qCritical(qUtf8Printable(QObject::tr("An error has occured during reply parse: %s. The reply text: \n%s")), qUtf8Printable(QObject::tr(e.what())), qUtf8Printable(data));
        if (_error_func)
            _error_func(Error::ReplyParseError, QNetworkReply::NetworkError::NoError);
    }
}

void FileSystemModel::handle_error(QNetworkReply::NetworkError error) {
    if (_error_func)
        _error_func(Error::NetworkError, error);
}
