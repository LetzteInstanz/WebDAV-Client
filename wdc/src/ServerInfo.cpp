#include "ServerInfo.h"

ServerInfo::ServerInfo(std::string&& description, std::string&& addr, std::uint16_t port, std::string&& path) noexcept
    : _description(std::move(description)), _addr(std::move(addr)), _port(port), _path(std::move(path)) {}

std::string ServerInfo::get_description() const { return _description; }

void ServerInfo::set_description(std::string&& desc) noexcept(std::is_nothrow_move_assignable<decltype(_description)>::value) { _description = std::move(desc); }

std::string ServerInfo::get_addr() const { return _addr; }

void ServerInfo::set_addr(std::string&& addr) noexcept(std::is_nothrow_move_assignable<decltype(_addr)>::value) { _addr = std::move(addr); }

uint16_t ServerInfo::get_port() const noexcept { return _port; }

void ServerInfo::set_port(uint16_t port) noexcept { _port = port; }

std::string ServerInfo::get_path() const { return _path; }

void ServerInfo::set_path(std::string&& path) noexcept(std::is_nothrow_move_assignable<decltype(_path)>::value) { _path = std::move(path); }
