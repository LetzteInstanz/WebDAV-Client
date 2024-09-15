#include "App.h"

#include "FileSystem/FileSystemModel.h"
#include "Json/SettingsJsonFile.h"
#include "Logger.h"
#include "Qml/FileSystemModel.h"
#include "Qml/IconProvider.h"
#include "Qml/ItemModelManager.h"
#include "Qml/Settings.h"
#include "Qml/Sort/SortParam.h"
#include "ServerInfo.h"
#include "ServerInfoManager.h"
#include "pch.h"

App::App(int& argc, char** argv) : QGuiApplication(argc, argv) {
    auto logger = Logger::get_instance();
    logger->install_handler();
    auto settings = std::make_shared<SettingsJsonFile>(logger);
    _qml_settings = std::make_unique<Qml::Settings>(settings);
    auto srv_mgr = std::make_unique<ServerInfoManager>();
    auto fs_model = std::make_shared<FileSystemModel>();
    _qml_fs_client = std::make_unique<Qml::FileSystemModel>(fs_model);
    _item_model_mgr = std::make_unique<Qml::ItemModelManager>(logger, settings, std::move(srv_mgr), fs_model);
}

App::~App() = default;

void App::initialize_engine(QQmlApplicationEngine& engine) {
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, this, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.addImageProvider("icons", new Qml::IconProvider());
    QQmlContext* context = engine.rootContext();
    //engine.setInitialProperties({{"srvItemModel", QVariant::fromValue(to_type<QObject*>(&_srv_item_model))}}); // note: Replace with this, when fixed https://bugreports.qt.io/browse/QTBUG-114403
    context->setContextProperty("settings", _qml_settings.get());
    context->setContextProperty("logLevelItemModel", _qml_settings->get_level_desc_list());
    context->setContextProperty("fileSystemModel", _qml_fs_client.get());
    context->setContextProperty("itemModelManager", _item_model_mgr.get());
}
