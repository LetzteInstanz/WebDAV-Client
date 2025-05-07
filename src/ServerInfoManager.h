#pragma once

#include <cstddef>
#include <vector>

#include "Json/DataJsonFile.h"

class ServerInfo;

class ServerInfoManager {
public:
    ServerInfoManager();

    ServerInfo get(std::size_t row) const noexcept;
    std::size_t amount() const noexcept;

    void add(ServerInfo&& info);
    void edit(std::size_t row, const ServerInfo& info);
    void remove(std::size_t row, std::size_t count);

private:
    DataJsonFile _json_file;
    std::vector<ServerInfo> _infos;
};
