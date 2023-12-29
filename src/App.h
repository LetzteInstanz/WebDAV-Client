#pragma once

#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

class Logger;
class ServerInfoManager;
class SettingsJsonFile;

namespace Qml {
    class QmlLogger;
    class QmlSettings;
    class ServerItemModel;
}

class App : public QGuiApplication {
public:
    App(int& argc, char** argv);
    ~App();

    void initialize_engine(QQmlApplicationEngine& engine);

private:
    std::shared_ptr<Logger> _logger;
    std::shared_ptr<SettingsJsonFile> _settings;
    std::shared_ptr<ServerInfoManager> _srv_info_manager;
    std::unique_ptr<Qml::QmlLogger> _qml_logger;
    std::unique_ptr<Qml::QmlSettings> _qml_settings;
    std::unique_ptr<Qml::ServerItemModel> _srv_item_model;
};
