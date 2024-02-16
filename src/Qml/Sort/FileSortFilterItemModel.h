#pragma once

#include <memory>
#include <vector>

#include <QModelIndex>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QSortFilterProxyModel>
#include <QTimer>

class SettingsJsonFile;

namespace Qml {
    class FileItemModel;
    class SortParam;

    class FileSortFilterItemModel : public QSortFilterProxyModel {
        Q_OBJECT

    public:
        FileSortFilterItemModel(std::shared_ptr<SettingsJsonFile> settings, std::unique_ptr<FileItemModel, QScopedPointerDeleteLater>&& source, QObject* parent = nullptr);
        ~FileSortFilterItemModel() override;

        Q_INVOKABLE void search(const QString& text);
        Q_INVOKABLE void searchWithTimer(const QString& text);
        Q_INVOKABLE void repeatSearch(int msec);

    protected:
        bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
        bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

    private:
        void update();

    private:
        std::shared_ptr<SettingsJsonFile> _settings;
        std::unique_ptr<FileItemModel, QScopedPointerDeleteLater> _source;

        std::vector<SortParam> _params;
        QTimer _timer;
        QString _text;
        bool _case_sensitive;
    };
}
