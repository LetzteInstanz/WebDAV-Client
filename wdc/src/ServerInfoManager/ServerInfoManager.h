#pragma once

class JsonFile;
class ServerInfo;

class ServerInfoManager final {
public:
    ServerInfoManager(std::shared_ptr<JsonFile> file);
    ~ServerInfoManager();

    ServerInfo get(std::size_t row) const;
    std::size_t amount() const noexcept;

    void add(ServerInfo&& info);
    void edit(std::size_t row, ServerInfo&& info);
    void remove(std::size_t row, std::size_t count);

private:
    class RootSection;

private:
    std::shared_ptr<JsonFile> _file;
    std::unique_ptr<RootSection> _root_section;
};
