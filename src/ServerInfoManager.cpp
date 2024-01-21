#include "ServerInfoManager.h"

ServerInfoManager::ServerInfoManager() { _infos = _json_file.read_servers(); }

ServerInfo ServerInfoManager::get(const size_t row) const { return _infos[row]; }

void ServerInfoManager::add(ServerInfo&& info) {
    _json_file.add(info);
    _infos.emplace_back(std::move(info));
}

void ServerInfoManager::edit(const size_t row, const ServerInfo& info) {
    _json_file.edit(row, info);
    _infos[row] = info;
}

void ServerInfoManager::remove(const size_t row, const size_t count) {
    _json_file.remove(row, count);
    const auto beg_it = std::cbegin(_infos);
    const auto first_it = beg_it + row;
    const auto end_it = first_it + count;
    _infos.erase(first_it, end_it);
}
