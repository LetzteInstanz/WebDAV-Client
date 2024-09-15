#include "ServerInfo.h"

#include "Util.h"
#include "pch.h"

const char* const ServerInfo::_desc_key = "description",
          * const ServerInfo::_addr_key = "address",
          * const ServerInfo::_port_key = "port",
          * const ServerInfo::_path_key = "path";

ServerInfo::ServerInfo(const QString& description, const QString& addr, uint16_t port, const QString& path) noexcept : _description(description), _addr(addr), _port(port), _path(path) {}

ServerInfo ServerInfo::from_json(const QJsonObject& obj) {
    const auto throw_ = []() { throw std::runtime_error("JSON parsing error for ServerInfo object"); };
    const auto to_str = [throw_](const QJsonObject& obj, const char* key) {
        const auto it = obj.find(key);
        if (it == std::end(obj)) {
            json_value_exist_warning(key);
            throw_();
        }
        if (!it->isString()) {
            json_value_type_warning(key, QObject::tr("a string"));
            throw_();
        }
        return it->toString();
    };
    const auto to_uint16 = [throw_](const QJsonObject& obj, const char* key) {
        const auto it = obj.find(key);
        if (it == std::end(obj)) {
            json_value_exist_warning(key);
            throw_();
        }
        if (!it->isDouble()) {
            json_value_type_warning(key, QObject::tr("a number"));
            throw_();
        }
        return it->toInteger();
    };
    return ServerInfo(to_str(obj, _desc_key), to_str(obj, _addr_key), to_uint16(obj, _port_key), to_str(obj, _path_key));
}

QJsonObject ServerInfo::to_json() const {
    QJsonObject obj;
    obj[_desc_key] = _description;
    obj[_addr_key] = _addr;
    obj[_port_key] = _port;
    obj[_path_key] = _path;
    return obj;
}
