#pragma once

#include <cstdint>
#include <deque>
#include <memory>

#include <QByteArray>
#include <QString>

#include "FileSystemObject.h"

class Client;
class Parser;

class FileSystemModel {
public:
    FileSystemModel();
    ~FileSystemModel();

    void set_server_info(const QString& addr, const uint16_t port, const QString& path);
    void request_file_list();
    void stop();

private:
    void handle_reply(QByteArray&& data);

private:
    std::unique_ptr<Client> _client;
    std::unique_ptr<Parser> _parser;
    QString _current_dir;
    std::deque<FileSystemObject> _objects;
};
