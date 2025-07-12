#include "ServerInfoManager.h"

#include "../ServerInfo.h"
#include "../Json/JsonFile.h"
#include "RootSection.h"

ServerInfoManager::ServerInfoManager(std::shared_ptr<JsonFile> file) : _file(file) {
    auto get_root_object = [this]() { return _file->get_root_object(); };
    auto set_root_object = [this](nlohmann::json&& object) { _file->set_root_object(std::move(object)); };
    _root_section = std::make_unique<RootSection>(std::move(get_root_object), std::move(set_root_object));
}

ServerInfoManager::~ServerInfoManager() = default;

ServerInfo ServerInfoManager::get(std::size_t row) const noexcept { return _root_section->get_server(row); }

std::size_t ServerInfoManager::amount() const noexcept { return _root_section->server_count(); }

void ServerInfoManager::add(ServerInfo&& info) { _root_section->add_server(std::move(info)); }

void ServerInfoManager::edit(std::size_t row, ServerInfo&& info) { _root_section->edit_server(row, std::move(info)); }

void ServerInfoManager::remove(std::size_t row, std::size_t count) { _root_section->remove_server(row, count); }
