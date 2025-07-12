#pragma once

#include "../Json/RootObject.h"
#include "ServerInfoManager.h"

class ServerInfo;

class ServerInfoManager::RootSection final : public RootObject {
public:
    RootSection(std::function<nlohmann::json ()>&& get_root_object, std::function<void (nlohmann::json&&)>&& set_root_object);
    ~RootSection() override;

    ServerInfo get_server(std::size_t row) const;
    std::size_t server_count() const noexcept;
    void add_server(ServerInfo&& info);
    void edit_server(std::size_t row, ServerInfo&& info);
    void remove_server(std::size_t row, std::size_t count);

private:
    std::vector<ServerInfo> _server_infos;
};
