#include "App.h"

#include "Json/SettingsJsonFile.h"
#include "Logger.h"
#include "Qml/Logger.h"
#include "Qml/ServerItemModel.h"
#include "Qml/Settings.h"
#include "ServerInfoManager.h"

App::App(int& argc, char** argv) : QGuiApplication(argc, argv) {
    _logger = Logger::get_instance();
    _settings = std::make_shared<SettingsJsonFile>(_logger);
    _srv_info_manager = std::make_shared<ServerInfoManager>();

    _qml_logger = std::make_unique<Qml::Logger>(_logger);
    _qml_settings = std::make_unique<Qml::Settings>(_settings);
    _srv_item_model = std::make_unique<Qml::ServerItemModel>(_srv_info_manager);
}

void App::initialize_engine(QQmlApplicationEngine& engine) {
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, this, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);

    QQmlContext* context = engine.rootContext();
    context->setContextProperty("srvItemModel", _srv_item_model.get());
    //engine.setInitialProperties({{"srvItemModel", QVariant::fromValue(static_cast<QObject*>(&_srv_item_model))}}); // note: Replace with this, when fixed https://bugreports.qt.io/browse/QTBUG-114403
    context->setContextProperty("settings", _qml_settings.get());
    context->setContextProperty("logger", _qml_logger.get());
    context->setContextProperty("logLevelModel", _qml_settings->get_level_desc_list());
}

App::~App() = default;
