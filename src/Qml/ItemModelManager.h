#pragma once

#include <memory>

#include <QAbstractItemModel>
#include <QObject>
#include <QQmlEngine>

class FileSystemModel;
class ServerInfoManager;
class SettingsJsonFile;

namespace Qml {
    class FileSortFilterItemModel;

    struct ItemModel {
        Q_GADGET
        QML_ELEMENT

    public:
        enum class Model {Server, File, SortParam};
        Q_ENUM(Model);
    };

    class ItemModelManager : public QObject {
        Q_OBJECT

    public:
        ItemModelManager(std::shared_ptr<SettingsJsonFile> settings, std::unique_ptr<ServerInfoManager>&& srv_mgr, std::shared_ptr<::FileSystemModel> fs_model, QObject* parent = nullptr);

        Q_INVOKABLE QAbstractItemModel* createModel(ItemModel::Model model);

    private:
        std::shared_ptr<SettingsJsonFile> _settings;
        std::shared_ptr<ServerInfoManager> _srv_mgr;
        std::shared_ptr<::FileSystemModel> _fs_model;

        Qml::FileSortFilterItemModel* _file_item_model = nullptr;
    };
}
