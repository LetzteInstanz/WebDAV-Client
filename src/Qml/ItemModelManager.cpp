#include "ItemModelManager.h"

#include "../FileSystem/FileSystemModel.h"
#include "../Json/SettingsJsonFile.h"
#include "../ServerInfo.h"
#include "../ServerInfoManager.h"
#include "../Util.h"
#include "FileItemModel/FileItemModel.h"
#include "ServerItemModel.h"
#include "Sort/FileSortFilterItemModel.h"
#include "Sort/SortParamItemModel.h"

Qml::ItemModelManager::ItemModelManager(std::shared_ptr<SettingsJsonFile> settings, std::unique_ptr<ServerInfoManager>&& srv_mgr, std::shared_ptr<::FileSystemModel> fs_model, QObject* parent)
    : QObject(parent), _settings(std::move(settings)), _srv_mgr(std::move(srv_mgr)), _fs_model(std::move(fs_model)) {}

QAbstractItemModel* Qml::ItemModelManager::createModel(ItemModel::Model model) {
    switch (model) {
        case ItemModel::Model::Server: {
            return new Qml::ServerItemModel(_srv_mgr);
        }

        case ItemModel::Model::File: {
            std::unique_ptr<Qml::FileItemModel, QScopedPointerDeleteLater> source(new Qml::FileItemModel(_fs_model));
            return new Qml::FileSortFilterItemModel(_settings, std::move(source));
        }

        case ItemModel::Model::SortParam: {
            return new Qml::SortParamItemModel(_settings);
        }

        default: {
            qCritical(qUtf8Printable(QObject::tr("Unknown item model type index: %d")), to_int(model));
            break;
        }
    }
    return nullptr;
}
