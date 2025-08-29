#include "FileItemModelFactory.h"

#include "../Settings/Settings.h"
#include "FileItemModel/FileItemModel.h"
#include "Sort/FileSortFilterItemModel.h"

Qml::FileItemModelFactory::FileItemModelFactory(std::shared_ptr<::Settings> settings, QObject* parent)
    : AbstractFileSystemModelFactory(parent), _settings(std::move(settings)) {}

Qml::FileItemModelFactory::~FileItemModelFactory() = default;

QObject* Qml::FileItemModelFactory::createModel() {
    std::unique_ptr<FileItemModel, QScopedPointerDeleteLater> source(new FileItemModel(get_fs_model()));
    return new FileSortFilterItemModel(_settings, std::move(source));
}
