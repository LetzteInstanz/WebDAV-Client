#include "FileItemModel.h"

#include "../Util.h"
#include "../FileSystem/FileSystemModel.h"

using namespace Qml;

FileItemModel::FileItemModel(std::shared_ptr<::FileSystemModel> model, QObject* parent) : QAbstractListModel(parent), _fs_model(model) {
    _fs_model->add_notification_func(this, std::bind(&FileItemModel::update, this));
}

FileItemModel::~FileItemModel() { _fs_model->remove_notification_func(this); }

int FileItemModel::rowCount(const QModelIndex& parent) const { return _fs_model->size(); }

QVariant FileItemModel::data(const QModelIndex& index, int role) const {
    if (role < to_int(Role::Name) || role > to_int(Role::Datetime))
        return QVariant();

    const FileSystemObject obj = _fs_model->get_object(index.row());
    switch (to_type<Role>(role)) {
        case Role::Name: {
            return obj.get_name();
        }

        case Role::Extension: {
            if (obj.get_type() == FileSystemObject::Type::Directory)
                return "folder";

            const QString name = obj.get_name();
            const qsizetype i = name.lastIndexOf('.');
            const qsizetype sz = name.size();
            if (i == -1 || i == sz - 1)
                return "unknown";

            return QStringView(name.begin() + i, sz - 1 - i).toString().toLower();
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
    }
    return QVariant();
}

QHash<int, QByteArray> FileItemModel::roleNames() const {
    auto names = QAbstractListModel::roleNames();
    names.emplace(to_int(Role::Name), "name");
    names.emplace(to_int(Role::Extension), "extension");
    names.emplace(to_int(Role::Datetime), "datetime");
    return names;
}

void FileItemModel::update() {
    beginResetModel();
    endResetModel();
}
