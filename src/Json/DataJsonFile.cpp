#include "DataJsonFile.h"

#include "../ServerInfo.h"

DataJsonFile::DataJsonFile() : JsonFile("data.json"), _servers_json(nlohmann::json::array()) {
    const nlohmann::json object = get_root_obj();
    const auto it = object.find(_server_key);
    if (it != std::end(object) && it->is_array())
        _servers_json = it->get<nlohmann::json::array_t>();
}

DataJsonFile::~DataJsonFile() {
    nlohmann::json object = get_root_obj();
    object[_server_key] = _servers_json;
    set_root_obj(std::move(object));
}

std::vector<ServerInfo> DataJsonFile::read_servers() const {
    std::vector<ServerInfo> servers;
    servers.reserve(_servers_json.size());
    for (auto it = std::begin(_servers_json); it != std::end(_servers_json); ++it) {
        if (!it->is_object())
            continue;

        try {
            const auto object = it->get<nlohmann::json::object_t>();
            servers.emplace_back(ServerInfo::from_json(object));
        } catch (const std::runtime_error&) {}
    }
    return servers;
}

void DataJsonFile::add(const ServerInfo& srv) { _servers_json.push_back(srv.to_json()); }

void DataJsonFile::edit(std::size_t index, const ServerInfo& srv) { _servers_json[index] = srv.to_json(); }

void DataJsonFile::remove(std::size_t row, std::size_t count) {
    auto it = std::next(std::cbegin(_servers_json), row);
    for (std::size_t i = 0; i < count; ++i)
        it = _servers_json.erase(it);
}
