#pragma once

#include <cstdint>

#include <QJsonObject>
#include <QString>

class ServerInfo {
public:
    ServerInfo(const QString& description, const QString& addr, const uint16_t port, const QString& path);

    QString get_description() const { return _description; }
    void set_description(const QString& desc) { _description = desc; }

    QString get_addr() const { return _addr; }
    void set_addr(const QString& addr) { this->_addr = addr; }

    uint16_t get_port() const { return _port; }
    void set_port(const uint16_t port) { this->_port = port; }

    QString get_path() const { return _path; }
    void set_path(const QString& path) { this->_path = path; }

    static ServerInfo from_json(const QJsonObject& json);
    QJsonObject to_json() const;

private:
    static const char* const _desc_key,
                     * const _addr_key,
                     * const _port_key,
                     * const _path_key;

    QString _description;
    QString _addr;
    uint16_t _port;
    QString _path;
};
