#pragma once

#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

namespace Qml {
    class FileSystemModel;
    class ItemModelManager;
    class Settings;
}

class App : public QGuiApplication {
public:
    App(int& argc, char** argv);
    ~App() override;

    void initialize_engine(QQmlApplicationEngine& engine);

private:
    std::unique_ptr<Qml::Settings> _qml_settings;
    std::unique_ptr<Qml::FileSystemModel> _qml_fs_client;
    std::unique_ptr<Qml::ItemModelManager> _item_model_mgr;
};
