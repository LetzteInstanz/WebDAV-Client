#include "App.h"

#include "FileSystem/FileSystemModel.h"
#include "Json/SettingsJsonFile.h"
#include "Logger.h"
#include "Qml/FileItemModel.h"
#include "Qml/FileSystemModel.h"
#include "Qml/Logger.h"
#include "Qml/ServerItemModel.h"
#include "Qml/Settings.h"
#include "ServerInfoManager.h"

App::App(int& argc, char** argv) : QGuiApplication(argc, argv) {
    auto logger = Logger::get_instance();
    _qml_logger = std::make_unique<Qml::Logger>(logger);
    auto settings = std::make_unique<SettingsJsonFile>(logger);
    _qml_settings = std::make_unique<Qml::Settings>(std::move(settings));
    auto srv_info_manager = std::make_unique<ServerInfoManager>();
    _srv_item_model = std::make_unique<Qml::ServerItemModel>(std::move(srv_info_manager));
    auto fs_model = std::make_shared<FileSystemModel>();
    _qml_fs_client = std::make_unique<Qml::FileSystemModel>(fs_model);
    _file_item_model = std::make_unique<Qml::FileItemModel>(fs_model);
}

App::~App() = default;

void App::initialize_engine(QQmlApplicationEngine& engine) {
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, this, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    QQmlContext* context = engine.rootContext();
    context->setContextProperty("srvItemModel", _srv_item_model.get());
    //engine.setInitialProperties({{"srvItemModel", QVariant::fromValue(static_cast<QObject*>(&_srv_item_model))}}); // note: Replace with this, when fixed https://bugreports.qt.io/browse/QTBUG-114403
    context->setContextProperty("settings", _qml_settings.get());
    context->setContextProperty("logger", _qml_logger.get());
    context->setContextProperty("logLevelModel", _qml_settings->get_level_desc_list());
    context->setContextProperty("fileSystemModel", _qml_fs_client.get());
}
