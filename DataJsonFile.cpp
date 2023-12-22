#include "DataJsonFile.h"

const char* const DataJsonFile::_server_key = "servers";

DataJsonFile::DataJsonFile() : JsonFile("data.json") {
    const QJsonObject obj = get_root_obj();
    const auto it = obj.find(_server_key);
    if (it != obj.end() && it->isArray())
        _json_servers = it->toArray();
}

std::vector<ServerInfo> DataJsonFile::read_servers() const {
    std::vector<ServerInfo> servers;
    servers.reserve(_json_servers.size());
    for (const QJsonValue& json_srv : _json_servers) {
        if (json_srv.isObject())
            servers.push_back(ServerInfo::from_json(json_srv.toObject()));
    }
    return servers;
}

void DataJsonFile::add(const ServerInfo& srv) {
    _json_servers.append(srv.to_json());
    write();
}

void DataJsonFile::edit(const size_t index, const ServerInfo& srv) {
    _json_servers[index] = srv.to_json();
    write();
}

void DataJsonFile::remove(const size_t row, const size_t count) {
    auto it = std::begin(_json_servers) + row;
    for (size_t i = 0; i < count; ++i)
        it = _json_servers.erase(it);

    write();
}

void DataJsonFile::write() {
    QJsonObject obj = get_root_obj();
    obj[_server_key] = _json_servers;
    write(obj);
}
