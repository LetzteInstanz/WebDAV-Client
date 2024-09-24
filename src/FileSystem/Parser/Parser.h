#pragma once

#include <cstddef>
#include <deque>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <QByteArray>
#include <QString>
#include <QStringView>

#include "../../Util.h"

class FileSystemObject;

class Parser {
public:
    using CurrDirObj = std::unique_ptr<FileSystemObject>;
    using Objects = std::deque<FileSystemObject>;
    using Result = std::pair<CurrDirObj, Objects>;

    static Result parse_propfind_reply(const QStringView& current_path, const QByteArray& data);

#ifndef NDEBUG
    static void test();
#endif

private:
    enum class Tag {None, Multistatus, Response, PropStat, Prop, Href, ResourceType, CreationDate, GetLastModified, Collection, GetContentLength, Status};

    struct TagHasher {
        std::size_t operator()(Tag t) const noexcept { return to_type<std::size_t>(t); }
    };
    using TagSet = std::unordered_set<Tag, TagHasher>;
    using TagOrderMap = std::unordered_map<Tag, TagSet, TagHasher>;
    struct CurrentState;

    static const std::unordered_map<QString, Tag> _propfind_tag_by_str_map;
    static const TagOrderMap _propfind_tag_order;
};
