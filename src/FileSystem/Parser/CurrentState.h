#pragma once

#include <stack>
#include <vector>

#include <QString>
#include <QStringView>

#include "FSObjectStruct.h"
#include "Parser.h"

struct Parser::CurrentState {
    CurrentState(const QStringView& current_path, TagOrderMap::const_iterator first, Result& result);

    void update_if_start_tag(Tag t);
    void update_if_end_tag(Tag t);
    void update_if_data(Tag t, const QStringView& data);

    bool was_error = false;
    std::stack<TagOrderMap::const_iterator, std::vector<TagOrderMap::const_iterator>> stack;
    QStringView not_dav_namespace;

private:
    void set_error(QString&& msg);

private:
    class TimeParser;

    const QStringView& _current_path;
    FSObjectStruct _obj;
    FSObjectStruct::Status _status = FSObjectStruct::Status::None;
    Result& _result;
};
