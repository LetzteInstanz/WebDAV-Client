#include "ServerItemModel.h"

#include "ServerInfoManager.h"
#include "Util.h"

ServerItemModel::ServerItemModel(ServerInfoManager& manager, QObject* parent) : QAbstractListModel(parent), _srv_manager(manager) {}

int ServerItemModel::rowCount(const QModelIndex& parent) const { return toInt(_srv_ids.size()); }

QVariant ServerItemModel::data(const QModelIndex& index, int role) const {
    if (role < toInt(Role::Desc) || role > toInt(Role::Path))
        return QVariant();

    const ServerInfo::Id id = _srv_ids[index.row()];
    const ServerInfo info = _srv_manager.get(id);
    switch (toType<Role>(role)) {
        case Role::Desc:
            return info.get_description();

        case Role::Addr:
            return info.get_addr();

        case Role::Port:
            return info.get_port();

        case Role::Path:
            return info.get_path();
    }
    return QVariant();
}

bool ServerItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (role < toInt(Role::Desc) || role > toInt(Role::Path))
        return false;

    const ServerInfo::Id id = _srv_ids[index.row()];
    ServerInfo info = _srv_manager.get(id);

    std::function<QString()> get;
    std::function<void(const QString&)> set;
    auto is_port = false;
    switch (toType<Role>(role)) {
        case Role::Desc: {
            get = std::bind(&ServerInfo::get_description, &info);
            set = std::bind(&ServerInfo::set_description, &info, std::placeholders::_1);
            break;
        }

        case Role::Addr: {
            get = std::bind(&ServerInfo::get_addr, &info);
            set = std::bind(&ServerInfo::set_addr, &info, std::placeholders::_1);
            break;
        }

        case Role::Port: {
            const uint16_t port = value.toUInt();
            if (info.get_port() == port)
                return false;

            info.set_port(port);
            is_port = true;
            break;
        }

        case Role::Path: {
            get = std::bind(&ServerInfo::get_path, &info);
            set = std::bind(&ServerInfo::set_path, &info, std::placeholders::_1);
            break;
        }
    }
    if (!is_port) {
        const QString str = value.toString();
        if (get() == str)
            return false;

        set(str);
    }
    _srv_manager[id] = info;
    dataChanged(index, index, {role});
    return true;
}

QVariant ServerItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
    /*if (orientation == Qt::Vertical)
        return QVariant();*/

    return "headerText";
}

Qt::ItemFlags ServerItemModel::flags(const QModelIndex& index) const {
    const auto flags = QAbstractListModel::flags(index);
    return flags | Qt::ItemIsEditable;
}

QHash<int, QByteArray> ServerItemModel::roleNames() const {
    auto names = QAbstractListModel::roleNames();
    names.emplace(toInt(Role::Desc), "desc");
    names.emplace(toInt(Role::Addr), "addr");
    names.emplace(toInt(Role::Port), "port");
    names.emplace(toInt(Role::Path), "path");
    return names;
}

void ServerItemModel::add_server_info(const QString& description, const QString& addr, const uint16_t port, const QString& path) {
    const int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    const ServerInfo::Id id = _srv_manager.add_new(ServerInfo(description, addr, port, path));
    _srv_ids.insert(_srv_ids.begin() + row, id);
    endInsertRows();
}
