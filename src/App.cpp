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
    qmlRegisterUncreatableType<Qml::ItemModel>("WebDavClient", 1, 0, "ItemModel", "This struct is for enum class");
    qmlRegisterSingletonInstance("WebDavClient", 1, 0, "Settings", _qml_settings.get());
    qmlRegisterSingletonInstance("WebDavClient", 1, 0, "FileSystemModel", _qml_fs_client.get());
    qmlRegisterSingletonInstance("WebDavClient", 1, 0, "ItemModelManager", _item_model_mgr.get());
}
