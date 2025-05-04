#pragma once

#include <cstdint>
#include <string>
#include <string_view>

class ServerInfo {
public:
    ServerInfo(std::string_view description, std::string_view addr, std::uint16_t port, std::string_view path) noexcept;

    std::string get_description() const noexcept { return _description; }
    void set_description(std::string_view desc) noexcept { _description = desc; }

    std::string get_addr() const noexcept { return _addr; }
    void set_addr(std::string_view addr) noexcept { _addr = addr; }

    std::uint16_t get_port() const noexcept { return _port; }
    void set_port(std::uint16_t port) noexcept { _port = port; }

    std::string get_path() const noexcept { return _path; }
    void set_path(std::string_view path) noexcept { _path = path; }

private:
    std::string _description;
    std::string _addr;
    std::uint16_t _port;
    std::string _path;
};
