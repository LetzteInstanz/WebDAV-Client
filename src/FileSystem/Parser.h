#pragma once

#include <cstddef>
#include <ctime>
#include <deque>
#include <memory>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <QByteArray>
#include <QChar>
#include <QString>
#include <QStringView>

#include "../Util.h"
#include "FileSystemObject.h"

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

    struct FSObjectStruct {
        using Status = FileSystemObject::Status;
        using Type = FileSystemObject::Type;

        static QString extract_name(const QStringView& abs_path);
        static Status to_status(const QStringView& str);

        void replace_unknown_status(Status s);

        bool is_curr_dir_obj = false;
        QString name;
        std::pair<Status, Type> type = {Status::None, Type::File};
        std::pair<Status, time_t> creation_date = {Status::None, 0};
        std::pair<Status, time_t> last_modified = {Status::None, 0};

    private:
        constexpr static Status ret_second_if_first_is_unknown(Status first, Status second);

    private:
        static const std::vector<std::pair<QString, Status>> _str_code_pairs;
    };

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
