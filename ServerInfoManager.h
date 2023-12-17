#pragma once

#include <unordered_map>

#include "ServerInfo.h"

class ServerInfoManager {
public:
    ServerInfo get(ServerInfo::Id id) const;

    ServerInfo::Id add_new(const ServerInfo& info);
    //void erase() {}

    ServerInfo& operator[](ServerInfo::Id id);

private:
    using IdServerInfoHash = std::unordered_map<ServerInfo::Id, ServerInfo>;

    const IdServerInfoHash::const_iterator _get(IdServerInfoHash::const_iterator it, ServerInfo::Id id) const;

private:
    ServerInfo::Id _max_id = 0;
    IdServerInfoHash _info_hash;
};
