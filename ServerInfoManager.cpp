#include "ServerInfoManager.h"

ServerInfoManager::ServerInfoManager() { _infos = _json_file.read_servers(); }

ServerInfo ServerInfoManager::get(const size_t row) const { return _infos[row]; }

void ServerInfoManager::add(const ServerInfo& info) {
    _infos.push_back(info);
    _json_file.add(info);
}

void ServerInfoManager::edit(const size_t row, const ServerInfo& info) {
    _infos[row] = info;
    _json_file.edit(row, info);
}

void ServerInfoManager::remove(const size_t row, const size_t count) {
    const auto beg_it = std::cbegin(_infos);
    const auto first_it = beg_it + row;
    const auto end_it = first_it + count;
    _infos.erase(first_it, end_it);
    _json_file.remove(row, count);
}
