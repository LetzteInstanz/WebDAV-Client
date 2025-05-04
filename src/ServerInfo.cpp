#include "ServerInfo.h"

ServerInfo::ServerInfo(std::string_view description, std::string_view addr, std::uint16_t port, std::string_view path) noexcept : _description(description), _addr(addr), _port(port), _path(path) {}
