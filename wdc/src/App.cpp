#include "App.h"

#include "Json/JsonFile.h"
#include "Settings/Settings.h"
#include "Logger.h"
#ifdef ANDROID
#include "NotificationClient.h"
#endif
#include "Qml/Factory/FileSystemModelFactory.h"
#include "Qml/Factory/LogItemModelFactory.h"
#include "Qml/Factory/ServerItemModelFactory.h"
#include "Qml/Factory/SortParamItemModelFactory.h"
#include "Qml/FileItemModel/Role.h"
#include "Qml/IconProvider.h"
#include "Qml/Settings.h"

App::App(int& argc, char** argv) : QGuiApplication(argc, argv) {
    auto logger = Logger::get_instance();
    logger->install_handler();
    auto config_file = std::make_shared<JsonFile>("config.json");
    auto settings = std::make_shared<Settings>(logger, config_file);
    _log_item_model_factory = std::make_unique<Qml::LogItemModelFactory>(std::move(logger));
    _qml_settings = std::make_unique<Qml::Settings>(settings);
    _sort_param_item_model_factory = std::make_unique<Qml::SortParamItemModelFactory>(settings);
    _server_item_model_factory = std::make_unique<Qml::ServerItemModelFactory>(std::move(config_file));
    _file_system_model_factory = std::make_unique<Qml::FileSystemModelFactory>(std::move(settings));
#ifdef ANDROID
    try {
        _notification_client = std::make_unique<NotificationClient>(QObject::tr("Downloading"));
    } catch (const std::runtime_error& e) {
        qWarning(qUtf8Printable(QObject::tr("NotificationClient: %s")), e.what());
    }
#endif
}

#ifdef ANDROID
App::~App() {
    if (_notification_client)
        _notification_client->hide_all_notifications();
}
#else
App::~App() = default;
#endif

void App::initialize_engine(QQmlApplicationEngine& engine) {
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, this, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    engine.addImageProvider("icons", new Qml::IconProvider());
    qmlRegisterUncreatableMetaObject(Qml::staticMetaObject, "WebDavClient", 1, 0, "Qml", "Qml is a namespace");
    qmlRegisterSingletonInstance("WebDavClient", 1, 0, "LogItemModelFactory", _log_item_model_factory.get());
    qmlRegisterSingletonInstance("WebDavClient", 1, 0, "Settings", _qml_settings.get());
    qmlRegisterSingletonInstance("WebDavClient", 1, 0, "SortParamItemModelFactory", _sort_param_item_model_factory.get());
    qmlRegisterSingletonInstance("WebDavClient", 1, 0, "ServerItemModelFactory", _server_item_model_factory.get());
    qmlRegisterSingletonInstance("WebDavClient", 1, 0, "FileSystemModelFactory", _file_system_model_factory.get());
    qmlRegisterType<Qml::FileSystemInfo>("WebDavClient", 1, 0, "FileSystemInfo");
}
