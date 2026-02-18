#include "ServerItemModelFactory.h"

#include "../ServerInfoManager/ServerInfoManager.h"
#include "ServerItemModel.h"

Qml::ServerItemModelFactory::ServerItemModelFactory(std::shared_ptr<JsonFile> config_file, QObject* parent) : QObject(parent), _config_file(std::move(config_file)) {}

Qml::ServerItemModelFactory::~ServerItemModelFactory() = default;

QObject* Qml::ServerItemModelFactory::createModel() { return new ServerItemModel(std::make_unique<ServerInfoManager>(_config_file)); }
