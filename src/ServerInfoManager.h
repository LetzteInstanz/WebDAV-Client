#pragma once

#include <cstddef>
#include <vector>

#include "Json/DataJsonFile.h"

class ServerInfo;

class ServerInfoManager {
public:
    ServerInfoManager();

    ServerInfo get(size_t row) const noexcept;
    size_t amount() const noexcept;

    void add(ServerInfo&& info);
    void edit(size_t row, const ServerInfo& info);
    void remove(size_t row, size_t count);

private:
    DataJsonFile _json_file;
    std::vector<ServerInfo> _infos;
};
