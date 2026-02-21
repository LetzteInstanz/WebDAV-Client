#include "FileSystemModelFactory.h"

#include "../Settings/Settings.h"
#include "FileSystem/DownloadFileSystemModel.h"
#include "FileItemModel/FileItemModel.h"
#include "FileSystem/MainFileSystemModel.h"
#include "Sort/FileSortFilterItemModel.h"

Qml::FileSystemModelFactory::FileSystemModelFactory(std::shared_ptr<::Settings> settings) : _settings(std::move(settings)) {}

Qml::FileSystemModelFactory::~FileSystemModelFactory() = default;

QObject* Qml::FileSystemModelFactory::createModel(const QString& addr, std::uint16_t port) {
    assert(_fs_model.expired() && _main_fs_model == nullptr);
    auto fs_model = std::make_shared<::FileSystemModel>(addr, port);
    _fs_model = fs_model;
    _main_fs_model = new MainFileSystemModel(std::move(fs_model), [this]() { _main_fs_model = nullptr; });
    return _main_fs_model;
}

QObject* Qml::FileSystemModelFactory::createModel() {
    assert(!_fs_model.expired());
    return new DownloadFileSystemModel(_fs_model.lock());
}

QObject* Qml::FileSystemModelFactory::createItemModel(const QString& root_path) {
    std::unique_ptr<FileItemModel, QScopedPointerDeleteLater> source(new FileItemModel(root_path.toStdString(), *_main_fs_model));
    return new FileSortFilterItemModel(_settings, std::move(source));
}
