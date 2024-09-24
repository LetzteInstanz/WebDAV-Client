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

class ServerInfoManager;

namespace Qml {
    class ServerItemModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit ServerItemModel(std::shared_ptr<ServerInfoManager> manager, QObject* parent = nullptr);
        ~ServerItemModel() override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

        bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;

        Qt::ItemFlags flags(const QModelIndex& index) const override;

        QHash<int, QByteArray> roleNames() const override;

        Q_INVOKABLE void addServerInfo(const QString& description, const QString& addr, std::uint16_t port, const QString& path);

    private:
        enum class Role {Desc = Qt::UserRole, Addr, Port, Path};

        std::shared_ptr<ServerInfoManager> _srv_manager;
    };
}
