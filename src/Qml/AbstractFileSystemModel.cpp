#include "AbstractFileSystemModel.h"

#include "../QtUtil.h"

Qml::AbstractFileSystemModel::AbstractFileSystemModel(std::shared_ptr<FileSystemModel> model, QString&& log_prefix, FileSystemModel::DataSet data_set, bool overwrite_result)
    : _fs_model(std::move(model)), _log_prefix(std::move(log_prefix)), _data_set(data_set), _overwrite_result(overwrite_result) {}

Qml::AbstractFileSystemModel::~AbstractFileSystemModel() { _fs_model->abort_request_sync(std::ranges::to<std::vector>(_uncompl_req_ids)); }

void Qml::AbstractFileSystemModel::requestData(const QString& absolute_path, bool recursive) {
    progressTextChanged(tr("Getting the list of resources…"));
    auto ready_func = [this](FileSystemModel::FsPair&& id_fs_pair) {
        invoke_method(*this, [this, fs = std::move(id_fs_pair)]() mutable { process_reply(std::move(fs)); });
    };
    auto error_func = [this](FileSystemModel::Id id, FileSystemModel::Error custom_error, QNetworkReply::NetworkError qt_error, bool is_critical) {
        invoke_method(*this, std::bind(&AbstractFileSystemModel::process_error, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4), id, custom_error, qt_error, is_critical);
    };
    auto request = std::make_tuple(absolute_path.toStdString(), _data_set, recursive, std::move(ready_func), std::move(error_func));
    const FileSystemModel::Id id = _fs_model->request_data(std::move(request));
    assert(!_uncompl_req_ids.contains(id));
    _uncompl_req_ids.emplace(id);
    qInfo(qUtf8Printable(QObject::tr("The resource %s is requested (ID: %d)")), qUtf8Printable(absolute_path), id);
}

void Qml::AbstractFileSystemModel::abortRequests() {
    FileSystemModel::IdVector new_ignored;
    _ignored_req_ids.reserve(_uncompl_req_ids.size());
    new_ignored.reserve(_uncompl_req_ids.size());
    for (const FileSystemModel::Id id : _uncompl_req_ids) {
        const auto it = _ignored_req_ids.find(id);
        if (it != std::ranges::cend(_ignored_req_ids))
            return;

        _ignored_req_ids.emplace_hint(it, id);
        new_ignored.emplace_back(id);
    }
    auto forget_ids = [this](FileSystemModel::IdVector&& ids) {
        invoke_method(*this, [this, ids = std::move(ids)]() { std::ranges::for_each(ids, [this](FileSystemModel::Id id) { _ignored_req_ids.erase(id); _uncompl_req_ids.erase(id); }); });
    };
    _fs_model->abort_request(std::move(new_ignored), std::move(forget_ids));
}

void Qml::AbstractFileSystemModel::process_error(FileSystemModel::Id id, FileSystemModel::Error custom_error, QNetworkReply::NetworkError qt_error, bool is_critical) {
    _uncompl_req_ids.erase(id);
    if (drop_ignore(id))
        return;

    if (custom_error == FileSystemModel::Error::ResponseParseError) {
        errorOccurred(id, QObject::tr("HTTP response parse error"), is_critical);
        return;
    }
    assert(qt_error != QNetworkReply::NoError);
    const QString display_str = to_qstring(qt_error);
    qCritical(qUtf8Printable(QObject::tr("Network error: %s (ID: %d)")), qUtf8Printable(display_str), id);
    errorOccurred(id, display_str, is_critical);
}

void Qml::AbstractFileSystemModel::process_reply(FileSystemModel::FsPair&& id_fs_pair) {
    const FileSystemModel::Id id = id_fs_pair.first;
    _uncompl_req_ids.erase(id);
    if (drop_ignore(id))
        return;

    if (_overwrite_result)
        _fs_by_id_map.clear();

#ifdef NDEBUG
    _fs_by_id_map.emplace(std::move(id_fs_pair));
#else
    assert(_fs_by_id_map.emplace(std::move(id_fs_pair)).second);
#endif
    ready(id);
}

bool Qml::AbstractFileSystemModel::drop_ignore(FileSystemModel::Id id) {
    const auto id_it = _ignored_req_ids.find(id);
    if (id_it == std::ranges::cend(_ignored_req_ids))
        return false;

    _ignored_req_ids.erase(id_it);
    qDebug(qUtf8Printable(QObject::tr("%1: response dropped (ID: %d)").arg(_log_prefix)), id);
    return true;
}
