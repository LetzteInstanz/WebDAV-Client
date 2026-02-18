#pragma once

class FileSystemModel;
class Settings;

namespace Qml {
    class MainFileSystemModel;

    class FileSystemModelFactory final : public QObject {
        Q_OBJECT

    public:
        FileSystemModelFactory(std::shared_ptr<::Settings> settings);
        ~FileSystemModelFactory();

        Q_INVOKABLE QObject* createModel(const QString& addr, std::uint16_t port);
        Q_INVOKABLE QObject* createModel();
        Q_INVOKABLE QObject* createItemModel(const QString& root_path);

    private:
        std::shared_ptr<::Settings> _settings;
        std::weak_ptr<FileSystemModel> _fs_model;
        MainFileSystemModel* _main_fs_model = nullptr;
    };
}
