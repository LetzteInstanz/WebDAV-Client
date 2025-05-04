#include "DataJsonFile.h"

#include "../ServerInfo.h"
#include "../Util.h"

namespace JsonKeys {
    const constexpr char* obj = "servers",
                        * desc = "description",
                        * addr = "address",
                        * port = "port",
                        * path = "path";
}

namespace nlohmann {
    template <>
    struct adl_serializer<ServerInfo> {
        static ServerInfo from_json(const json& json) {
            auto desc = get_value_from_json<std::string>(json, JsonKeys::desc);
            auto addr = get_value_from_json<std::string>(json, JsonKeys::addr);
            const auto port = get_value_from_json<std::uint16_t>(json, JsonKeys::port);
            auto path = get_value_from_json<std::string>(json, JsonKeys::path);
            return ServerInfo(std::move(desc), std::move(addr), port, std::move(path));
        }

        static void to_json(json& json, const ServerInfo& info) {
            json = nlohmann::json{{JsonKeys::desc, info.get_description()}, {JsonKeys::addr, info.get_addr()}, {JsonKeys::port, info.get_port()}, {JsonKeys::path, info.get_path()}};
        }
    };
}

DataJsonFile::DataJsonFile() : JsonFile("data.json") {}

DataJsonFile::~DataJsonFile() = default;

std::vector<ServerInfo> DataJsonFile::read_servers() {
    nlohmann::json json = get_root_obj();
    try {
        json = json.at(JsonKeys::obj).get<nlohmann::json::array_t>();
    } catch (const nlohmann::json::exception& e) {
        qWarning(qUtf8Printable(QObject::tr("data.json: %s: %s")), JsonKeys::obj, e.what());
        json = nlohmann::json::array();
        set_value(JsonKeys::obj, std::as_const(json));
    }
    std::vector<ServerInfo> servers;
    servers.reserve(json.size());
    bool hasInvalid = false;
    for (auto it = std::cbegin(json); it != std::cend(json); ++it) {
        try {
            servers.emplace_back(it->get<ServerInfo>());
        } catch (const nlohmann::json::exception& /*e*/) {
            hasInvalid = true;
        }
    }
    if (hasInvalid)
        set_value(JsonKeys::obj, std::as_const(servers));

    return servers;
}

void DataJsonFile::add(const ServerInfo& srv) {
    nlohmann::json json = get_root_obj().at(JsonKeys::obj).get<nlohmann::json::array_t>();
    json.push_back(srv);
    set_value(JsonKeys::obj, std::move(json));
}

void DataJsonFile::edit(std::size_t index, const ServerInfo& srv) {
    nlohmann::json json = get_root_obj().at(JsonKeys::obj).get<nlohmann::json::array_t>();
    json[index] = srv;
    set_value(JsonKeys::obj, std::move(json));
}

void DataJsonFile::remove(std::size_t row, std::size_t count) {
    nlohmann::json json = get_root_obj().at(JsonKeys::obj).get<nlohmann::json::array_t>();
    auto it = std::next(std::cbegin(json), row);
    for (std::size_t i = 0; i < count; ++i)
        it = json.erase(it);

    set_value(JsonKeys::obj, std::move(json));
}
