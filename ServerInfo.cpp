#include "ServerInfo.h"

const char* const ServerInfo::_desc_key = "description",
          * const ServerInfo::_addr_key = "address",
          * const ServerInfo::_port_key = "port",
          * const ServerInfo::_path_key = "path";

ServerInfo::ServerInfo(const QString& description, const QString& addr, const uint16_t port, const QString& path) : _description(description), _addr(addr), _port(port), _path(path) {}

ServerInfo ServerInfo::from_json(const QJsonObject& obj) {
    const auto throw_except = [](const char* const key) { throw std::runtime_error(QObject::tr("Cannot get value by key \"").toStdString() + key + '"'); };
    const auto to_str = [throw_except](const QJsonObject& obj, const char* const key) {
        const auto it = obj.find(key);
        if (it == obj.end() || !it->isString())
            throw_except(key);

        return it->toString();
    };
    const auto to_uint16 = [throw_except](const QJsonObject& obj, const char* const key) {
        const auto it = obj.find(key);
        if (it == obj.end() || !it->isDouble())
            throw_except(key);

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
