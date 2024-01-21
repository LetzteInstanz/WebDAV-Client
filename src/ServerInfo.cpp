#include "ServerInfo.h"

const char* const ServerInfo::_desc_key = "description",
          * const ServerInfo::_addr_key = "address",
          * const ServerInfo::_port_key = "port",
          * const ServerInfo::_path_key = "path";

ServerInfo::ServerInfo(const QString& description, const QString& addr, const uint16_t port, const QString& path) : _description(description), _addr(addr), _port(port), _path(path) {}

ServerInfo ServerInfo::from_json(const QJsonObject& obj) {
    const auto log_1 = [](const char* const key) { qWarning(qUtf8Printable(QObject::tr("The value of the key \"%s\" doesn't exist")), key); };
    const auto log_2 = [](const char* const key, const QString& value_type) { qWarning(qUtf8Printable(QObject::tr("The value of the key \"%s\" isn't %s")), key, qUtf8Printable(value_type)); };
    const auto throw_ = []() { throw std::runtime_error("JSON parsing error for ServerInfo object"); };
    const auto to_str = [log_1, log_2, throw_](const QJsonObject& obj, const char* const key) {
        const auto it = obj.find(key);
        if (it == std::end(obj)) {
            log_1(key);
            throw_();
        }
        if (!it->isString()) {
            log_2(key, QObject::tr("a string"));
            throw_();
        }
        return it->toString();
    };
    const auto to_uint16 = [log_1, log_2, throw_](const QJsonObject& obj, const char* const key) {
        const auto it = obj.find(key);
        if (it == std::end(obj) || !it->isDouble()) {
            log_1(key);
            throw_();
        }
        if (!it->isDouble()) {
            log_2(key, QObject::tr("a number"));
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
