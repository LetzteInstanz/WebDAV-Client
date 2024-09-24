#pragma once

#include <cstddef>
#include <vector>

#include <QJsonArray>

#include "JsonFile.h"

class ServerInfo;

class DataJsonFile final : public JsonFile {
public:
    DataJsonFile();
    ~DataJsonFile() override;

    std::vector<ServerInfo> read_servers() const;
    void add(const ServerInfo& srv);
    void edit(std::size_t index, const ServerInfo& srv);
    void remove(std::size_t row, std::size_t count);

private:
    static const char* const _server_key;

    QJsonArray _json_servers;
};
