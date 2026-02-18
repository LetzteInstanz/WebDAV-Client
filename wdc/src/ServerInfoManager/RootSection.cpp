#include "RootSection.h"

#include "../ServerInfo.h"

namespace JsonKeys {
    const constexpr char* array = "servers";
    const constexpr char* desc = "description";
    const constexpr char* addr = "address";
    const constexpr char* port = "port";
    const constexpr char* path = "path";
}

namespace nlohmann {
    template <>
    struct adl_serializer<ServerInfo> {
        static ServerInfo from_json(const json& json) {
            constexpr const char* title = "ServerInfo";
            auto desc = get_value_from_json<std::string>(json, title, JsonKeys::desc);
            auto addr = get_value_from_json<std::string>(json, title, JsonKeys::addr);
            const auto port = get_value_from_json<std::uint16_t>(json, title, JsonKeys::port);
            auto path = get_value_from_json<std::string>(json, title, JsonKeys::path);
            return ServerInfo(std::move(desc), std::move(addr), port, std::move(path));
        }

        static void to_json(json& json, const ServerInfo& info) {
            json = nlohmann::json{{JsonKeys::desc, info.get_description()}, {JsonKeys::addr, info.get_addr()}, {JsonKeys::port, info.get_port()}, {JsonKeys::path, info.get_path()}};
        }
    };
}

ServerInfoManager::RootSection::RootSection(std::function<nlohmann::json ()>&& get_root_object, std::function<void (nlohmann::json&&)>&& set_root_object)
    : RootObject(std::move(get_root_object), std::move(set_root_object))
{
    nlohmann::json root = _get_root_object();
    bool value_changed;
    nlohmann::json array = *get_value<nlohmann::json::array_t>(root, []() { return nlohmann::json::array(); }, JsonKeys::array, value_changed);
    for (auto it = std::cbegin(array); it != std::cend(array);) {
        try {
            _server_infos.emplace_back(it->get<ServerInfo>());
            ++it;
        } catch (const nlohmann::json::exception& e) {
            qWarning(qUtf8Printable(QObject::tr("ServerInfoManager::RootSection::RootSection(): %s: %s")), JsonKeys::array, e.what());
            it = array.erase(it);
            value_changed = true;
        }
    }
    if (value_changed) {
        root[JsonKeys::array] = array;
        _set_root_object(std::move(root));
    }
}

ServerInfoManager::RootSection::~RootSection() = default;

ServerInfo ServerInfoManager::RootSection::get_server(std::size_t row) const { return _server_infos[row]; }

std::size_t ServerInfoManager::RootSection::server_count() const noexcept { return _server_infos.size(); }

void ServerInfoManager::RootSection::add_server(ServerInfo&& info) {
    _server_infos.emplace_back(info);
    nlohmann::json root = _get_root_object();
    root[JsonKeys::array].emplace_back(std::move(info));
    _set_root_object(std::move(root));
}

void ServerInfoManager::RootSection::edit_server(std::size_t row, ServerInfo&& info) {
    assert(row <_server_infos.size());
    auto& curr_info = _server_infos[row];
    if (curr_info == info)
        return;

    curr_info = info;
    nlohmann::json root = _get_root_object();
    root[JsonKeys::array][row] = std::move(info);
    _set_root_object(std::move(root));
}

void ServerInfoManager::RootSection::remove_server(std::size_t row, std::size_t count) {
    {
        const auto first_it = std::cbegin(_server_infos) + row;
        _server_infos.erase(first_it, first_it + count);
    }
    nlohmann::json root = _get_root_object();
    const auto array_it = root.find(JsonKeys::array);
    assert(array_it != std::cend(root) && array_it->is_array());
    const auto first_it = std::cbegin(*array_it) + row;
    array_it->erase(first_it, first_it + count);
    _set_root_object(std::move(root));
}
