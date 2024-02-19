#pragma once

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
#include <QString>
#include <QVariant>
#include <Qt>

class FileSystemModel;
class FileSystemObject;

namespace Qml {
    class FileItemModel : public QAbstractListModel {
        Q_OBJECT

    public:
        explicit FileItemModel(std::shared_ptr<::FileSystemModel> model, QObject* parent = nullptr);
        ~FileItemModel() override;

        int rowCount(const QModelIndex& parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

        QHash<int, QByteArray> roleNames() const override;

    private:
        FileSystemObject get_object(bool root_path, int row) const;
        QString get_icon_name(const FileSystemObject& obj, int row) const;
        void update();

    private:
        static const std::unordered_map<QString, QString> _icon_name_by_extension_map;
        static const std::unordered_set<QString> _special_icon_name_set;

        std::shared_ptr<::FileSystemModel> _fs_model;
        bool _root;
    };
}
