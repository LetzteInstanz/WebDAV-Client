#pragma once

#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#ifdef ANDROID
class NotificationClient;
#endif

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
#ifdef ANDROID
    std::unique_ptr<NotificationClient> _notification_client;
#endif
};
