#include "AbstractFileSystemModelFactory.h"

#include "../../FileSystem/Client.h"
#include "../../FileSystem/FileSystemModel.h"

std::weak_ptr<Client> Qml::AbstractFileSystemModelFactory::_client;
std::weak_ptr<::FileSystemModel> Qml::AbstractFileSystemModelFactory::_fs_model;

Qml::AbstractFileSystemModelFactory::~AbstractFileSystemModelFactory() = default;

std::shared_ptr<::FileSystemModel> Qml::AbstractFileSystemModelFactory::create_main_fs_model(QStringView addr, std::uint16_t port) {
    const auto create_new = _client.expired();
    auto client = create_new ? std::make_shared<::Client>(addr, port) : _client.lock();
    if (create_new)
        _client = client;

    auto fs_model = std::make_shared<::FileSystemModel>(std::move(client), ::FileSystemModel::State::Single);
    _fs_model = fs_model;
    return fs_model;
}

std::shared_ptr<::FileSystemModel> Qml::AbstractFileSystemModelFactory::create_fs_model() {
    assert(!_client.expired());
    return std::make_shared<::FileSystemModel>(_client.lock(), ::FileSystemModel::State::Multiple);
}

std::shared_ptr<::FileSystemModel> Qml::AbstractFileSystemModelFactory::get_main_fs_model() { assert(!_fs_model.expired()); return _fs_model.lock(); }
