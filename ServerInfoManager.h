#pragma once

#include <cstddef>
#include <vector>

#include "DataJsonFile.h"
#include "ServerInfo.h"

class ServerInfoManager {
public:
    ServerInfoManager();

    ServerInfo get(const size_t row) const;
    size_t amount() const { return _infos.size(); }

    void add(const ServerInfo& info);
    void edit(const size_t row, const ServerInfo& info);
    void remove(const size_t row, const size_t count);

private:
    DataJsonFile _json_file;
    std::vector<ServerInfo> _infos;
};
