#include "ServerInfoManager.h"

#include "ServerInfo.h"

ServerInfoManager::ServerInfoManager() : _infos(_json_file.read_servers()) {}

ServerInfo ServerInfoManager::get(std::size_t row) const noexcept { return _infos[row]; }

std::size_t ServerInfoManager::amount() const noexcept { return _infos.size(); }

void ServerInfoManager::add(ServerInfo&& info) {
    _json_file.add(info);
    _infos.emplace_back(std::move(info));
}

void ServerInfoManager::edit(std::size_t row, const ServerInfo& info) {
    _json_file.edit(row, info);
    _infos[row] = info;
}

void ServerInfoManager::remove(std::size_t row, std::size_t count) {
    _json_file.remove(row, count);
    const auto beg_it = std::cbegin(_infos);
    const auto first_it = beg_it + row;
    const auto end_it = first_it + count;
    _infos.erase(first_it, end_it);
}
