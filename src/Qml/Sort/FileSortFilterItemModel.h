#pragma once

#include <memory>

#include <QModelIndex>
#include <QObject>
#include <QScopedPointer>
#include <QSortFilterProxyModel>

class SettingsJsonFile;

namespace Qml {
    class FileItemModel;

    class FileSortFilterItemModel : public QSortFilterProxyModel {
        Q_OBJECT

    public:
        FileSortFilterItemModel(std::shared_ptr<SettingsJsonFile> settings, std::unique_ptr<FileItemModel, QScopedPointerDeleteLater>&& source, QObject* parent = nullptr);
        ~FileSortFilterItemModel() override;

    protected:
        bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

    private:
        std::shared_ptr<SettingsJsonFile> _settings;
        std::unique_ptr<FileItemModel, QScopedPointerDeleteLater> _source;
    };
}
