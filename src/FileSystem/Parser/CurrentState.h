#pragma once

#include "FSObjectStruct.h"
#include "Parser.h"

struct Parser::CurrentState {
    CurrentState(const std::filesystem::path& current_path, TagOrderMap::const_iterator first, Result& result);

    void update_if_start_tag(Tag t);
    void update_if_end_tag(Tag t);
    void update_if_data(Tag t, QStringView data);

    bool was_error = false;
    std::stack<TagOrderMap::const_iterator, std::vector<TagOrderMap::const_iterator>> stack;
    QStringView not_dav_namespace;

private:
    void set_error(QString&& msg);

private:
    class TimeParser;

    const std::filesystem::path& _current_path;
    FSObjectStruct _obj;
    std::optional<FSObjectStruct::Status> _status;
    Result& _result;
};
