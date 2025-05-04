#pragma once

#include <cstddef>
#include <vector>

#include "JsonFile.h"

class ServerInfo;

class DataJsonFile final : public JsonFile {
public:
    DataJsonFile();
    ~DataJsonFile() override;

    std::vector<ServerInfo> read_servers();
    void add(const ServerInfo& srv);
    void edit(std::size_t index, const ServerInfo& srv);
    void remove(std::size_t row, std::size_t count);
};
