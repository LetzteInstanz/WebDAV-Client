#pragma once

class JsonFile;

namespace Qml {
    class ServerItemModelFactory : public QObject {
        Q_OBJECT

    public:
        ServerItemModelFactory(std::shared_ptr<JsonFile> config_file, QObject* parent = nullptr);
        ~ServerItemModelFactory() override;

        Q_INVOKABLE QObject* createModel();

    private:
        std::shared_ptr<JsonFile> _config_file;
    };
}
