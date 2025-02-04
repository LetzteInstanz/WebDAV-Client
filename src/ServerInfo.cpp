#include "ServerInfo.h"

#include "Util.h"

ServerInfo::ServerInfo(std::string_view description, std::string_view addr, std::uint16_t port, std::string_view path) noexcept : _description(description), _addr(addr), _port(port), _path(path) {}

ServerInfo ServerInfo::from_json(const nlohmann::json& object) {
    const auto throw_ = []() { throw std::runtime_error("JSON parsing error for ServerInfo object"); };
    const auto to_str = [throw_](const nlohmann::json& object, const char* key) {
        const auto it = object.find(key);
        if (it == std::end(object)) {
            json_value_exist_warning(key);
            throw_();
        }
        if (!it->is_string()) {
            json_value_type_warning(key, QObject::tr("a string"));
            throw_();
        }
        return it->get<std::string>();
    };
    const auto to_uint16 = [throw_](const nlohmann::json& object, const char* key) {
        const auto it = object.find(key);
        if (it == std::end(object)) {
            json_value_exist_warning(key);
            throw_();
        }
        if (!it->is_number_unsigned()) {
            json_value_type_warning(key, QObject::tr("a number"));
            throw_();
        }
        const auto number = it->get<std::uint16_t>();
        if (number == 0) {
            json_value_type_warning(key, QObject::tr("a number greater than 0"));
            throw_();
        }
        return number;
    };
    return ServerInfo(to_str(object, _desc_key), to_str(object, _addr_key), to_uint16(object, _port_key), to_str(object, _path_key));
}

nlohmann::json ServerInfo::to_json() const {
    nlohmann::json object = nlohmann::json::object();
    object[_desc_key] = _description;
    object[_addr_key] = _addr;
    object[_port_key] = _port;
    object[_path_key] = _path;
    return object;
}
