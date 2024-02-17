#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <QByteArray>
#include <QString>
#include <QStringView>

#include "../Util.h"
#include "Parser/FSObjectStruct.h"

class FileSystemObject;

class Parser {
public:
    using CurrDirObj = std::unique_ptr<FileSystemObject>;
    using Objects = std::deque<FileSystemObject>;
    using Result = std::pair<CurrDirObj, Objects>;

    static Result parse_propfind_reply(const QStringView& current_path, const QByteArray& data);

private:
    enum class Tag {None, Multistatus, Response, PropStat, Prop, Href, ResourceType, CreationDate, GetLastModified, Collection, Status};

    struct TagHasher {
        size_t operator()(Tag t) const noexcept { return to_type<size_t>(t); }
    };
    using TagSet = std::unordered_set<Tag, TagHasher>;
    using TagOrderMap = std::unordered_map<Tag, TagSet, TagHasher>;

    struct CurrentState {
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
        const QStringView& _current_path;
        FSObjectStruct _obj;
        FSObjectStruct::Status _status = FSObjectStruct::Status::None;
        Result& _result;
    };

#ifndef NDEBUG
public:
    static void test();
#endif

private:
    static const std::unordered_map<QString, Tag> _propfind_tag_by_str_map;
    static const TagOrderMap _propfind_tag_order;
};
