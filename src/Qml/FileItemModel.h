#pragma once

#include <cstdint>
#include <memory>

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QVariant>
#include <Qt>

class FileSystemModel;

namespace Qml {
    class FileItemModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit FileItemModel(std::shared_ptr<::FileSystemModel> model, QObject* parent = nullptr);

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

        //QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

        bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

        Qt::ItemFlags flags(const QModelIndex& index) const override;

        QHash<int, QByteArray> roleNames() const override;

        //Q_INVOKABLE void add_server_info(const QString& description, const QString& addr, const uint16_t port, const QString& path);

    private:
        enum class Role { Desc = Qt::UserRole, Addr, Port, Path };

        std::shared_ptr<FileSystemModel> _fs_model;
    };
}
