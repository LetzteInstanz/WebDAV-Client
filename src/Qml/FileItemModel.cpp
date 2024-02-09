#include "FileItemModel.h"

#include "../FileSystem/FileSystemModel.h"
#include "../FileSystem/FileSystemObject.h"
#include "../Util.h"

using namespace Qml;

FileItemModel::FileItemModel(std::shared_ptr<::FileSystemModel> model, QObject* parent) : QAbstractListModel(parent), _fs_model(model) {
    _fs_model->add_notification_func(this, std::bind(&FileItemModel::update, this));
}

FileItemModel::~FileItemModel() { _fs_model->remove_notification_func(this); }

int FileItemModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid() || !_fs_model->is_initialized())
        return 0;

    return _fs_model->size() + (_root ? 0 : 1);
}

QVariant FileItemModel::data(const QModelIndex& index, int role) const {
    if (role < to_int(Role::Name) || role > to_int(Role::FileFlag))
        return QVariant();

    const int row = index.row();
    const FileSystemObject obj = get_object(_root, row);
    switch (to_type<Role>(role)) {
        case Role::Name: {
            return !_root && row == 0 ? ".." : obj.get_name();
        }

        case Role::Extension: {
            if (obj.get_type() == FileSystemObject::Type::Directory || !_root && row == 0)
                return "folder";

            const QString name = obj.get_name();
            const qsizetype i = name.lastIndexOf('.');
            if (i == -1 || i == name.size() - 1)
                return "unknown";

            return QStringView(std::begin(name) + i + 1, std::end(name)).toString().toLower();
        }

        case Role::Datetime: {
            if (!obj.is_last_modified_valid())
                return QObject::tr("unknown");

            std::tm tm = obj.get_last_modified();
#ifdef Q_OS_WIN // todo: replace with std::gmtime(), when the compiler will support that
            const std::time_t t = _mkgmtime(&tm);
#else
            const std::time_t t = timegm(&tm);
#endif
            tm = *std::localtime(&t);
            std::ostringstream stream;
            //stream.imbue(std::locale("ru_RU.utf8")); // todo: take into account the translation setting, when it will be introduced
            stream << std::put_time(&tm, "%c");
            return QString::fromStdString(stream.rdbuf()->str());
        }

        case Role::FileFlag: {
            return obj.get_type() == FileSystemObject::Type::File;
        }
    }
    return QVariant();
}

QHash<int, QByteArray> FileItemModel::roleNames() const {
    auto names = QAbstractListModel::roleNames();
    names.emplace(to_int(Role::Name), "name");
    names.emplace(to_int(Role::Extension), "extension");
    names.emplace(to_int(Role::Datetime), "datetime");
    names.emplace(to_int(Role::FileFlag), "isFile");
    return names;
}

FileSystemObject FileItemModel::get_object(bool root_path, int row) const {
    if (row == 0)
        return root_path ? _fs_model->get_object(row) : _fs_model->get_curr_dir_object();

    return _fs_model->get_object(row - (root_path ? 0 : 1));
}

void FileItemModel::update() {
    beginResetModel();
    _root = _fs_model->is_cur_dir_root_path();
    endResetModel();
}
