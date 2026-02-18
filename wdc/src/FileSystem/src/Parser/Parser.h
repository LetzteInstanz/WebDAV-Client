#pragma once

#include "../../Util.h"
#include "FSObjectStruct.h"

class FileSystemObject;

class Parser {
public:
    using CurrDirObj = std::optional<FileSystemObject>;
    using Objects = std::deque<FileSystemObject>;
    using Result = std::pair<CurrDirObj, Objects>;

    class Exception : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
        ~Exception() override;
    };

    Parser(const std::filesystem::path& current_path);

    std::filesystem::path get_current_path() const;
    bool has_error() const;
    Result&& get_result();
    void parse_response_portion(const ReadBuffer& data);

private:
    void handle_token(QXmlStreamReader::TokenType token);

private:
    enum class Tag {None, Multistatus, Response, PropStat, Prop, Href, ResourceType, CreationDate, GetLastModified, Collection, GetContentLength, Status};

    struct TagHash {
        std::size_t operator()(Tag t) const noexcept { return to_type<std::size_t>(t); }
    };
    using TagSet = std::unordered_set<Tag, TagHash>;
    using TagOrderMap = std::unordered_map<Tag, TagSet, TagHash>;

    struct CurrentState {
        CurrentState(const std::filesystem::path& current_path, Result& result);

        void process_start_of_tag(Tag t);
        void process_data(Tag t, QStringView data);
        void process_end_of_tag(Tag t);

        bool has_error = false;
        const std::filesystem::path current_path;
        std::stack<TagOrderMap::const_iterator, std::vector<TagOrderMap::const_iterator>> stack;
        QStringView not_dav_namespace;

    private:
        void set_error(QString&& msg);

    private:
        class TimeParser;

        Result& _result;
        FSObjectStruct _obj;
        std::optional<FSObjectStruct::Status> _status;
    };

    static const std::unordered_map<QString, Tag> _propfind_tag_by_str_map;
    static const TagOrderMap _propfind_tag_order;

    Result _result;
    CurrentState _state;
    QXmlStreamReader _reader;
    std::string _critical_error_text;
    mutable std::ostringstream _response_text_stream;
};
