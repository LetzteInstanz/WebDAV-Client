module;

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <chrono>
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

#include "../FileSystemObject.h"
#include "../Util.h"

export module Parser;

export class Parser {
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

module : private;

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
    struct FSObjectStruct {
        using Status = FileSystemObject::Status;
        using Type = FileSystemObject::Type;

        static QString extract_name(const QStringView& abs_path);
        static Status to_status(const QStringView& str);

        void replace_unknown_status(Status s);

        bool is_curr_dir_obj = false;
        QString name;
        std::pair<Status, Type> type = {Status::None, Type::File};
        std::pair<Status, std::chrono::sys_seconds> creation_date = {Status::None, {}};
        std::pair<Status, std::chrono::sys_seconds> last_modified = {Status::None, {}};
        std::pair<Status, std::uint64_t> content_length = {Status::None, 0};

    private:
        constexpr static Status ret_second_if_first_is_unknown(Status first, Status second);

    private:
        static const std::vector<std::pair<QString, Status>> _str_code_pairs;
    };

    const QStringView& _current_path;
    FSObjectStruct _obj;
    FSObjectStruct::Status _status = FSObjectStruct::Status::None;
    Result& _result;
};

class TimeParser {
public:
    enum class Format {Rfc2616, Rfc3339};

    static std::chrono::sys_seconds to_sys_seconds(const QStringView& str, Format f);

#ifndef NDEBUG
    static void test();
#endif

private:
    struct CustomTime {
        std::chrono::year year;
        std::chrono::month month;
        std::chrono::day day;
        std::chrono::hours hours;
        std::chrono::minutes minutes;
        std::chrono::seconds seconds;
        int time_zone_sign;
    };
    enum class Token {DayName, Day, Month, MonthName, Year, Hours, Minutes, Seconds, ZoneHours, ZoneMinutes};
    using CharSet = std::unordered_set<QChar>;
    using TokenOrder = std::vector<Token>;

    static const CharSet& get_delimiters(Format f);
    static const TokenOrder& get_order(const QStringView& str, Format f);
    static void parse(CustomTime& time, const QStringView& lexem, bool& ok, Token token);

private:
    static const CharSet _rfc2616_delimiters;
    static const CharSet _rfc3339_delimiters;
    static const std::unordered_map<QString, std::chrono::month> _month_map;
    static const TokenOrder _rfc2616_order_1;
    static const TokenOrder _rfc2616_order_2;
    static const TokenOrder _rfc3339_order_1;
    static const TokenOrder _rfc3339_order_2;
};
