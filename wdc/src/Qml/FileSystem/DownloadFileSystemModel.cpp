#include "DownloadFileSystemModel.h"

Qml::DownloadFileSystemModel::DownloadFileSystemModel(std::shared_ptr<FileSystemModel> model) : AbstractFileSystemModel(std::move(model), "Qml::DownloadFileSystemModel", FileSystemModel::DataSet::Basic, false) {
    qDebug().noquote().nospace() << QObject::tr("Qml::DownloadFileSystemModel: created");
}

Qml::DownloadFileSystemModel::~DownloadFileSystemModel() { qDebug().noquote().nospace() << QObject::tr("Qml::DownloadFileSystemModel: destroyed"); }

void Qml::DownloadFileSystemModel::remove(FileSystemModel::Id id) { assert(_fs_by_id_map.contains(id)); _fs_by_id_map.erase(id); }

void Qml::DownloadFileSystemModel::count(FileSystemModel::Id id, FileSystemInfo* source) const {
    assert(source);
    assert(_fs_by_id_map.contains(id));
    const auto& [_, objects] = _fs_by_id_map.at(id);
    for (const FileSystemObject& object : objects) {
        const auto is_dir = object.get_type() == FileSystemObject::Type::Directory;
        std::size_t& counter = is_dir ? source->directoryCount : source->fileCount;
        ++counter;
        const std::optional<std::uint64_t>& size = object.get_size();
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
