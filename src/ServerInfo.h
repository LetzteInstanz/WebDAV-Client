#pragma once

#include <cstdint>

#include <QJsonObject>
#include <QString>

class ServerInfo {
public:
    ServerInfo(const QString& description, const QString& addr, std::uint16_t port, const QString& path) noexcept;

    QString get_description() const noexcept { return _description; }
    void set_description(const QString& desc) noexcept { _description = desc; }

    QString get_addr() const noexcept { return _addr; }
    void set_addr(const QString& addr) noexcept { _addr = addr; }

    std::uint16_t get_port() const noexcept { return _port; }
    void set_port(std::uint16_t port) noexcept { _port = port; }

    QString get_path() const noexcept { return _path; }
    void set_path(const QString& path) noexcept { _path = path; }

    static ServerInfo from_json(const QJsonObject& json);
    QJsonObject to_json() const;

private:
    static const char* const _desc_key,
                     * const _addr_key,
                     * const _port_key,
                     * const _path_key;

    QString _description;
    QString _addr;
    std::uint16_t _port;
    QString _path;
};
