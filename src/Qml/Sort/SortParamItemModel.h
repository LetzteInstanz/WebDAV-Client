#pragma once

#include <memory>
#include <vector>

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <Qt>

class SettingsJsonFile;

namespace Qml {
    class FileSortFilterItemModel;
    class SortParam;

    class SortParamItemModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit SortParamItemModel(std::shared_ptr<SettingsJsonFile> settings, std::shared_ptr<Qml::FileSortFilterItemModel> file_model, QObject* parent = nullptr);
        ~SortParamItemModel() override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE bool hasChanges() const;
        Q_INVOKABLE void moveUp(int row);
        Q_INVOKABLE void invert();
        Q_INVOKABLE void moveDown(int row);
        Q_INVOKABLE void save();
        Q_INVOKABLE void resetChanges();

    private:
        enum class Role {Descending = Qt::UserRole};

        std::shared_ptr<SettingsJsonFile> _settings;
        std::shared_ptr<Qml::FileSortFilterItemModel> _file_model;
        std::vector<SortParam> _data;
    };
}
