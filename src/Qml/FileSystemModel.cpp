#include "FileSystemModel.h"

#include "../QtUtil.h"
#include "FileItemModel/SizeDisplayer.h"

Qml::FileSystemInfo::~FileSystemInfo() {
    qDebug().noquote().nospace() << QObject::tr("Qml::FileSystemInfo: destroyed");
}

QString Qml::FileSystemInfo::getSizeStr() const { return SizeDisplayer::to_string(size); }

Qml::FileSystemModel::FileSystemModel(std::shared_ptr<::FileSystemModel> model) : _fs_model(std::move(model)) {
    qDebug().noquote().nospace() << QObject::tr("Qml::FileSystemModel: created");
    _fs_model->set_error_func(std::bind(&FileSystemModel::handle_error, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    _fs_model->add_notification_func(this, std::bind(&FileSystemModel::ready, this, std::placeholders::_1));
}

Qml::FileSystemModel::~FileSystemModel() {
    qDebug().noquote().nospace() << QObject::tr("Qml::FileSystemModel: destroyed");
    _fs_model->remove_notification_func(this);
    _fs_model->set_error_func(nullptr);
}

std::uint32_t Qml::FileSystemModel::requestFullData(const QString& path, bool recursive) {
    progressTextChanged(tr("Getting the list of resources…"));
    qInfo(qUtf8Printable(QObject::tr("The resource %s is requested")), path.isEmpty() ? "/" : qUtf8Printable(path));
    return _fs_model->request_data(path.toStdString(), ::FileSystemModel::DataSet::Full, recursive);
}

std::uint32_t Qml::FileSystemModel::requestBasicData(const QString& path, bool recursive) {
    progressTextChanged(tr("Getting the list of resources…"));
    qDebug(qUtf8Printable(QObject::tr("Qml::FileSystemModel: the resource %s is requested")), path.isEmpty() ? "/" : qUtf8Printable(path));
    return _fs_model->request_data(path.toStdString(), ::FileSystemModel::DataSet::Basic, recursive);
}

void Qml::FileSystemModel::remove(std::uint32_t file_system_id) { _fs_model->remove(file_system_id); }

void Qml::FileSystemModel::abortRequest(std::uint32_t file_system_id) { _fs_model->abort_request(file_system_id); }

void Qml::FileSystemModel::abortAllRequests() { _fs_model->abort_all_requests(); }

QString Qml::FileSystemModel::getCurrentPath(std::uint32_t file_system_id) const { return QString::fromStdString(_fs_model->get_curr_dir_object(file_system_id).get_path().generic_string()); }

void Qml::FileSystemModel::count(std::uint32_t file_system_id, FileSystemInfo* source) const {
    assert(source);
    const std::size_t count = _fs_model->get_count(file_system_id);
    for (std::size_t i = 0; i < count; ++i) {
        const FileSystemObject object = _fs_model->get_object(file_system_id, i);
        const auto is_dir = object.get_type() == FileSystemObject::Type::Directory;
        std::size_t& counter = is_dir ? source->directoryCount : source->fileCount;
        ++counter;
        const std::optional<uint64_t> size = object.get_size();
        if (size) {
            source->areInvalidAllSizes = false;
            source->size += *size;
        }
        if (!size && !is_dir) {
            source->areValidAllSizes = false;
            const std::string name = object.get_name();
            qInfo(qUtf8Printable(QObject::tr("File \"%s\" hasn't size")), name.c_str());
        }
    }
}

void Qml::FileSystemModel::handle_error(std::uint32_t file_system_id, ::FileSystemModel::Error custom_error, QNetworkReply::NetworkError qt_error, bool is_critical) {
    if (custom_error == ::FileSystemModel::Error::ResponseParseError) {
        errorOccurred(file_system_id, QObject::tr("HTTP response parse error"), is_critical);
        return;
    }
    assert(qt_error != QNetworkReply::NoError);
    const QString display_str = to_qstring(qt_error);
    qCritical(qUtf8Printable(QObject::tr("Network error: %s")), qUtf8Printable(display_str));
    errorOccurred(file_system_id, display_str, is_critical);
}
