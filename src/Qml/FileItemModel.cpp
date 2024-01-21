#include "FileItemModel.h"

#include "../Util.h"
#include "../FileSystem/FileSystemModel.h"

using namespace Qml;

FileItemModel::FileItemModel(std::shared_ptr<::FileSystemModel> model, QObject* parent) : QAbstractListModel(parent), _fs_model(model) {}

int FileItemModel::rowCount(const QModelIndex& parent) const { return 0;/*to_int(_srv_manager->amount());*/ }

QVariant FileItemModel::data(const QModelIndex& index, int role) const {
    if (role < to_int(Role::Desc) || role > to_int(Role::Path))
        return QVariant();

    // const ServerInfo info = _srv_manager->get(index.row());
    // switch (to_type<Role>(role)) {
    //     case Role::Desc:
    //         return info.get_description();

    //     case Role::Addr:
    //         return info.get_addr();

    //     case Role::Port:
    //         return info.get_port();

    //     case Role::Path:
    //         return info.get_path();
    // }
    return QVariant();
}

bool FileItemModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    // if (role < to_int(Role::Desc) || role > to_int(Role::Path))
        return false;

    // ServerInfo info = _srv_manager->get(index.row());
    // std::function<QString()> get;
    // std::function<void(const QString&)> set;
    // auto is_port = false;
    // switch (to_type<Role>(role)) {
    //     case Role::Desc: {
    //         get = std::bind(&ServerInfo::get_description, &info);
    //         set = std::bind(&ServerInfo::set_description, &info, std::placeholders::_1);
    //         break;
    //     }

    //     case Role::Addr: {
    //         get = std::bind(&ServerInfo::get_addr, &info);
    //         set = std::bind(&ServerInfo::set_addr, &info, std::placeholders::_1);
    //         break;
    //     }

    //     case Role::Port: {
    //         const uint16_t port = value.toUInt();
    //         if (info.get_port() == port)
    //             return false;

    //         info.set_port(port);
    //         is_port = true;
    //         break;
    //     }

    //     case Role::Path: {
    //         get = std::bind(&ServerInfo::get_path, &info);
    //         set = std::bind(&ServerInfo::set_path, &info, std::placeholders::_1);
    //         break;
    //     }
    // }
    // if (!is_port) {
    //     const auto str = value.toString();
    //     if (get() == str)
    //         return false;

    //     set(str);
    // }
    // _srv_manager->edit(index.row(), info);
    // dataChanged(index, index, {role});
    // return true;
}

// QVariant FileItemModel::headerData(int section, Qt::Orientation orientation, int role) const {
//     if (orientation == Qt::Vertical)
//         return QVariant();

//     return "headerText";
// }

bool FileItemModel::removeRows(int row, int count, const QModelIndex& /*parent*/) {
    // const auto last = row + count - 1;
    // beginRemoveRows(QModelIndex(), row, last);
    // _srv_manager->remove(row, count);
    // endRemoveRows();
    return true;
}

Qt::ItemFlags FileItemModel::flags(const QModelIndex& index) const {
    const auto flags = QAbstractListModel::flags(index);
    return flags | Qt::ItemIsEditable;
}

QHash<int, QByteArray> FileItemModel::roleNames() const {
    auto names = QAbstractListModel::roleNames();
    names.emplace(to_int(Role::Desc), "desc");
    names.emplace(to_int(Role::Addr), "addr");
    names.emplace(to_int(Role::Port), "port");
    names.emplace(to_int(Role::Path), "path");
    return names;
}

/*void FileItemModel::add_server_info(const QString& description, const QString& addr, const uint16_t port, const QString& path) {
    const int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    _srv_manager->add(ServerInfo(description, addr, port, path));
    endInsertRows();
}*/
