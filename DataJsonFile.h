#pragma once

#include <cstddef>
#include <vector>

#include <QJsonArray>

#include "JsonFile.h"
#include "ServerInfo.h"

class DataJsonFile final : public JsonFile {
public:
    DataJsonFile();

    std::vector<ServerInfo> read_servers() const;
    void add(const ServerInfo& srv);
    void edit(const size_t index, const ServerInfo& srv);
    void remove(const size_t row, const size_t count);

private:
    using JsonFile::write;

    void write();

private:
    static const char* const _server_key;

    QJsonArray _json_servers;
};
