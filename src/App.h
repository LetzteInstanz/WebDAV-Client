#pragma once

#ifdef ANDROID
class NotificationClient;
#endif

namespace Qml {
    class FileSystemModelFactory;
    class LogItemModelFactory;
    class ServerItemModelFactory;
    class Settings;
    class SortParamItemModelFactory;
}

class App : public QGuiApplication {
public:
    App(int& argc, char** argv);
    ~App() override;

    void initialize_engine(QQmlApplicationEngine& engine);

private:
    std::unique_ptr<Qml::LogItemModelFactory> _log_item_model_factory;
    std::unique_ptr<Qml::Settings> _qml_settings;
    std::unique_ptr<Qml::SortParamItemModelFactory> _sort_param_item_model_factory;
    std::unique_ptr<Qml::ServerItemModelFactory> _server_item_model_factory;
    std::unique_ptr<Qml::FileSystemModelFactory> _file_system_model_factory;
#ifdef ANDROID
    std::unique_ptr<NotificationClient> _notification_client;
#endif
};
