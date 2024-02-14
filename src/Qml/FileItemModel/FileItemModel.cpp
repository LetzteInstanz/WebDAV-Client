#include "FileItemModel.h"

#include "../FileSystem/FileSystemModel.h"
#include "../FileSystem/FileSystemObject.h"
#include "../Util.h"
#include "FileItemModel/Role.h"

using namespace Qml;

namespace Qml {
    using Role = FileItemModelRole;
}

namespace {
    time_t to_time_t(std::tm& tm) {
#ifdef Q_OS_WIN // todo: replace with std::gmtime(), when the compiler will support that
        return _mkgmtime(&tm);
#else
        return timegm(&tm);
#endif
    }

    bool is_valid_dot_pos(const QStringView& name, qsizetype pos) { return pos != -1 && pos != name.size() - 1; }

    QString extract_extension(const QStringView& name, qsizetype dot_pos) { return QStringView(std::begin(name) + dot_pos + 1, std::end(name)).toString().toLower(); }
}

FileItemModel::FileItemModel(std::shared_ptr<::FileSystemModel> model, QObject* parent) : QAbstractListModel(parent), _fs_model(model) {
    _fs_model->add_notification_func(this, std::bind(&FileItemModel::update, this));
}

FileItemModel::~FileItemModel() { _fs_model->remove_notification_func(this); }

int FileItemModel::rowCount(const QModelIndex& parent) const { return parent.isValid() ? 0 : _fs_model->size() + (_root ? 0 : 1); }

QVariant FileItemModel::data(const QModelIndex& index, int role) const {
    if (role < to_int(Role::Name) || role >= to_int(Role::Size))
        return QVariant();

    const int row = index.row();
    switch (to_type<Role>(role)) {
        case Role::Name: {
            const FileSystemObject obj = get_object(_root, row);
            return !_root && row == 0 ? ".." : obj.get_name();
        }

        case Role::Extension: {
            const FileSystemObject obj = get_object(_root, row);
            if (obj.get_type() == FileSystemObject::Type::Directory)
                return QVariant();

            const QString name = obj.get_name();
            const qsizetype i = name.lastIndexOf('.');
            if (!is_valid_dot_pos(name, i))
                return QVariant();

            return extract_extension(name, i);
        }

        case Role::IconName: {
            const FileSystemObject obj = get_object(_root, row);
            if (obj.get_type() == FileSystemObject::Type::Directory || !_root && row == 0)
                return QStringLiteral("folder");

            const QString name = obj.get_name();
            const qsizetype i = name.lastIndexOf('.');
            if (!is_valid_dot_pos(name, i))
                return QStringLiteral("unknown");

            return extract_extension(name, i);
        }

        case Role::ModTime: {
            const FileSystemObject obj = get_object(_root, row);
            if (!obj.is_last_modified_valid())
                return QVariant::fromValue(std::numeric_limits<time_t>::lowest());

            if (obj.get_name() == "Database4.kdb")
                return QVariant::fromValue(std::numeric_limits<time_t>::lowest());

            std::tm tm = obj.get_last_modified();
            return QVariant::fromValue(to_time_t(tm));
        }

        case Role::ModTimeStr: {
            const FileSystemObject obj = get_object(_root, row);
            if (!obj.is_last_modified_valid())
                return QObject::tr("unknown");

            std::tm tm = obj.get_last_modified();
            const std::time_t t = to_time_t(tm);
            tm = *std::localtime(&t);
            std::ostringstream stream;
            //stream.imbue(std::locale("ru_RU.utf8")); // todo: take into account the translation setting, when it will be introduced
            stream << std::put_time(&tm, "%c");
            return QString::fromStdString(stream.rdbuf()->str());
        }

        case Role::FileFlag: {
            const FileSystemObject obj = get_object(_root, row);
            return obj.get_type() == FileSystemObject::Type::File;
        }

        case Role::IsExit: {
            return !_root && row == 0;
        }

        default:
            break;
    }
    return QVariant();
}

QHash<int, QByteArray> FileItemModel::roleNames() const {
    auto names = QAbstractListModel::roleNames();
    names.emplace(to_int(Role::Name), "name");
    names.emplace(to_int(Role::Extension), "extension");
    names.emplace(to_int(Role::IconName), "iconName");
    names.emplace(to_int(Role::ModTimeStr), "modificationTime");
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
