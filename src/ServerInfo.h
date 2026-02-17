#pragma once

class ServerInfo {
public:
    ServerInfo(std::string&& description, std::string&& addr, std::uint16_t port, std::string&& path) noexcept;

    bool operator==(const ServerInfo& rhs) const noexcept = default;

    std::string get_description() const;
    void set_description(std::string&& desc) noexcept(std::is_nothrow_move_assignable<decltype(_description)>::value);

    std::string get_addr() const;
    void set_addr(std::string&& addr) noexcept(std::is_nothrow_move_assignable<decltype(_addr)>::value);

    std::uint16_t get_port() const noexcept;
    void set_port(std::uint16_t port) noexcept;

    std::string get_path() const;
    void set_path(std::string&& path) noexcept(std::is_nothrow_move_assignable<decltype(_path)>::value);

private:
    std::string _description;
    std::string _addr;
    std::uint16_t _port;
    std::string _path; // todo: replace with std::filesystem::path
};
