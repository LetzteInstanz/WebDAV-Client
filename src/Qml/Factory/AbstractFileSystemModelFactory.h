#pragma once

class Client;
class FileSystemModel;

namespace Qml {
    class AbstractFileSystemModelFactory : public QObject {
        Q_OBJECT

    public:
        using QObject::QObject;
        ~AbstractFileSystemModelFactory() override;

    protected:
        std::shared_ptr<::FileSystemModel> create_main_fs_model(QStringView addr, std::uint16_t port);
        std::shared_ptr<::FileSystemModel> create_fs_model();
        std::shared_ptr<::FileSystemModel> get_main_fs_model();

    private:
        static std::weak_ptr<Client> _client;
        static std::weak_ptr<::FileSystemModel> _fs_model;
    };
}
