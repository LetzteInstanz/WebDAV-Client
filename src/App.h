#pragma once

#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

namespace Qml {
    class FileItemModel;
    class FileSystemModel;
    class Logger;
    class ServerItemModel;
    class Settings;
}

class App : public QGuiApplication {
public:
    App(int& argc, char** argv);
    ~App();

    void initialize_engine(QQmlApplicationEngine& engine);

private:
    std::unique_ptr<Qml::Logger> _qml_logger;
    std::unique_ptr<Qml::Settings> _qml_settings;
    std::unique_ptr<Qml::ServerItemModel> _srv_item_model;
    std::unique_ptr<Qml::FileItemModel> _file_item_model;
    std::unique_ptr<Qml::FileSystemModel> _qml_fs_client;
};
