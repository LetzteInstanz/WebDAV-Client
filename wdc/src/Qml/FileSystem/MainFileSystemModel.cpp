#include "MainFileSystemModel.h"

Qml::MainFileSystemModel::MainFileSystemModel(std::shared_ptr<FileSystemModel> model, std::function<void ()>&& on_destroy_func)
    : AbstractFileSystemModel(std::move(model), "Qml::MainFileSystemModel", FileSystemModel::DataSet::Full, true), _on_destroy_func(std::move(on_destroy_func))
{
    qDebug().noquote().nospace() << QObject::tr("Qml::MainFileSystemModel: created");
}

Qml::MainFileSystemModel::~MainFileSystemModel() {
    _on_destroy_func();
    qDebug().noquote().nospace() << QObject::tr("Qml::MainFileSystemModel: destroyed");
}

QString Qml::MainFileSystemModel::getCurrentPath() const { return QString::fromStdString(get_pair().first.get_path().generic_string()); }

const FileSystemObject& Qml::MainFileSystemModel::get_curr_dir_object() const { return get_pair().first; }

const FileSystemObject& Qml::MainFileSystemModel::get_object(std::size_t index) const { return get_pair().second[index]; }

std::size_t Qml::MainFileSystemModel::get_count() const noexcept { return get_pair().second.size(); }

const FileSystemModel::ObjectPair& Qml::MainFileSystemModel::get_pair() const noexcept {
    assert(_fs_by_id_map.size() == 1);
    return std::ranges::begin(_fs_by_id_map)->second;
}
