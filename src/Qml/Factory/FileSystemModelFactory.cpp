#include "FileSystemModelFactory.h"

#include "MainFileSystemModel.h"

Qml::FileSystemModelFactory::~FileSystemModelFactory() = default;

QObject* Qml::FileSystemModelFactory::createModel(const QString& addr, std::uint16_t port) { return new MainFileSystemModel(create_main_fs_model(addr, port)); }

QObject* Qml::FileSystemModelFactory::createModel() { return new MainFileSystemModel(create_fs_model()); }
