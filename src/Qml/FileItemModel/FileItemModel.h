#pragma once

#include <memory>

#include <QAbstractListModel>
#include <QByteArray>
#include <QHash>
#include <QModelIndex>
#include <QObject>
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
        void update();

    private:
        std::shared_ptr<::FileSystemModel> _fs_model;
        bool _root;
    };
}
