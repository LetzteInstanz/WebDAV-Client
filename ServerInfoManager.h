#pragma once

#include <unordered_map>

#include "ServerInfo.h"

class ServerInfoManager {
public:
    ServerInfo get(const ServerInfo::Id id) const;

    ServerInfo::Id add_new(const ServerInfo& info);
    void remove(const ServerInfo::Id id);

    ServerInfo& operator[](const ServerInfo::Id id);

private:
    using IdServerInfoHash = std::unordered_map<ServerInfo::Id, ServerInfo>;

    IdServerInfoHash::const_iterator _get(const IdServerInfoHash::const_iterator it, const ServerInfo::Id id) const;

private:
    ServerInfo::Id _max_id = 0;
    IdServerInfoHash _info_hash;
};
