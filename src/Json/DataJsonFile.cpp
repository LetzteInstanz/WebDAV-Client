#include "DataJsonFile.h"

#include "../ServerInfo.h"
#include "../pch.h"

const char* const DataJsonFile::_server_key = "servers";

DataJsonFile::DataJsonFile() : JsonFile("data.json") {
    const QJsonObject obj = get_root_obj();
    const auto it = obj.find(_server_key);
    if (it != std::end(obj) && it->isArray())
        _json_servers = it->toArray();
}

DataJsonFile::~DataJsonFile() {
    QJsonObject obj = get_root_obj();
    obj[_server_key] = _json_servers;
    set_root_obj(std::move(obj));
}

std::vector<ServerInfo> DataJsonFile::read_servers() const {
    std::vector<ServerInfo> servers;
    servers.reserve(_json_servers.size());
    for (const QJsonValue& json_srv : _json_servers) {
        if (!json_srv.isObject())
            continue;

        try {
            servers.emplace_back(ServerInfo::from_json(json_srv.toObject()));
        } catch (const std::runtime_error&) {}
    }
    return servers;
}

void DataJsonFile::add(const ServerInfo& srv) { _json_servers.append(srv.to_json()); }

void DataJsonFile::edit(size_t index, const ServerInfo& srv) { _json_servers[index] = srv.to_json(); }

void DataJsonFile::remove(size_t row, size_t count) {
    auto it = std::next(std::begin(_json_servers), row);
    for (size_t i = 0; i < count; ++i)
        it = _json_servers.erase(it);
}
