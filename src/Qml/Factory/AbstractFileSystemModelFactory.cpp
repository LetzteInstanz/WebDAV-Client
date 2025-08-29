#include "AbstractFileSystemModelFactory.h"

#include "../../FileSystem/FileSystemModel.h"

std::weak_ptr<::FileSystemModel> Qml::AbstractFileSystemModelFactory::_fs_model;

Qml::AbstractFileSystemModelFactory::~AbstractFileSystemModelFactory() = default;

std::shared_ptr<::FileSystemModel> Qml::AbstractFileSystemModelFactory::create_fs_model(QStringView addr, std::uint16_t port, QStringView root_path) {
    auto fs_model = std::make_shared<::FileSystemModel>(addr, port, root_path);
    _fs_model = fs_model;
    return fs_model;
}

std::shared_ptr<::FileSystemModel> Qml::AbstractFileSystemModelFactory::get_fs_model() { assert(!_fs_model.expired()); return _fs_model.lock(); }
