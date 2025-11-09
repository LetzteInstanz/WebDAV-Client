#include "Parser.h"

#include "../FileSystemObject.h"
#include "TimeParser.h"

Parser::Exception::~Exception() = default;

Parser::CurrentState::CurrentState(const std::filesystem::path& current_path, Result& result) : current_path(current_path), _result(result) {
    assert(!current_path.empty());
    assert(!current_path.has_filename());
    stack.push(_propfind_tag_order.find(Tag::None));
}

void Parser::CurrentState::process_start_of_tag(Tag t) {
    switch (t) {
        case Tag::Response:
            _obj = {};
            break;

        case Tag::PropStat:
            _status = FSObjectStruct::Status::Unknown;
            break;

        case Tag::ResourceType:
            _obj.type = std::make_pair(_status, FSObjectStruct::Type::File);
            break;

        case Tag::CreationDate:
            _obj.creation_date.first = _status;
            break;

        case Tag::GetLastModified:
            _obj.last_modified.first = _status;
            break;

        case Tag::Collection:
            _obj.type = std::make_pair(_status, FSObjectStruct::Type::Directory);
            break;

        case Tag::GetContentLength:
            _obj.content_length.first = _status;
            break;

        default:
            break;
    }
}

void Parser::CurrentState::process_data(Tag t, QStringView data) {
    switch (t) {
        case Tag::Href: {
            if (data.isEmpty())
                throw Exception("href XML element's data is empty");

            _obj.path = std::filesystem::path(QUrl::fromPercentEncoding(data.toLatin1()).toStdString());
            break;
        }

        case Tag::CreationDate: {
            try {
                _obj.creation_date = std::make_pair(_status, TimeParser::to_sys_seconds(data, TimeParser::Format::Rfc3339));
            } catch (const Exception& e) {
                set_error(QObject::tr(e.what()));
            }
            break;
        }

        case Tag::GetLastModified: {
            try {
                _obj.last_modified = std::make_pair(_status, TimeParser::to_sys_seconds(data, TimeParser::Format::Rfc2068));
            } catch (const Exception& e) {
                set_error(QObject::tr(e.what()));
            }
            break;
        }

        case Tag::GetContentLength: {
            bool ok;
            const auto length = data.toULongLong(&ok);
            if (ok)
                _obj.content_length.second = length;
            else
                set_error(QObject::tr("getcontentlength property isn't a number"));

            break;
        }

        case Tag::Status: {
            if (data.isEmpty())
                throw Exception("status XML element's data is empty");

            _status = FSObjectStruct::to_status(data);
            break;
        }

        default:
            break;
    }
}

void Parser::CurrentState::process_end_of_tag(Tag t) {
    switch (t) {
        case Tag::PropStat: {
            assert(_status.has_value());
            if (_status == FSObjectStruct::Status::Unknown)
                throw Exception("propstat XML element hasn't status element");

            _obj.replace_unknown_status(_status);
            _status.reset();
            break;
        }

        case Tag::Response: {
            if (!_obj.path.has_value())
                throw Exception("there isn't href XML element");

            if (!_obj.type.second.has_value())
                throw Exception("there isn't resourcetype property");

            if (_obj.type.first != FSObjectStruct::Status::Ok)
                throw Exception("resourcetype property hasn't ok status");

            const auto is_dir = _obj.type.second == FSObjectStruct::Type::Directory;
            const auto is_current_dir = _obj.path->has_filename() ? (*_obj.path / std::filesystem::path()) == current_path : _obj.path == current_path;
            if (!is_dir && is_current_dir)
                throw Exception("current directory is not collection");

            if (is_dir && _obj.path->has_filename())
                *_obj.path /= std::filesystem::path();

            if (!is_dir && !_obj.path->has_filename())
                _obj.path = _obj.path->parent_path();

            FileSystemObject obj(std::move(*_obj.path), std::move(_obj.creation_date.second), std::move(_obj.last_modified.second), std::move(_obj.content_length.second));
            if (is_current_dir)
                _result.first = std::move(obj);
            else
                _result.second.emplace_back(std::move(obj));

            break;
        }

        default:
            break;
    }
}

void Parser::CurrentState::set_error(QString&& msg) {
    has_error = true;
    qWarning(qUtf8Printable(QObject::tr("HTTP response parse error: %s.")), qUtf8Printable(msg));
}

const std::unordered_map<QString, Parser::Tag> Parser::_propfind_tag_by_str_map{{"multistatus", Tag::Multistatus},
                                                                                {"response", Tag::Response},
                                                                                {"propstat", Tag::PropStat},
                                                                                {"prop", Tag::Prop},
                                                                                {"href", Tag::Href},
                                                                                {"resourcetype", Tag::ResourceType},
                                                                                {"creationdate", Tag::CreationDate},
                                                                                {"getlastmodified", Tag::GetLastModified},
                                                                                {"collection", Tag::Collection},
                                                                                {"getcontentlength", Tag::GetContentLength},
                                                                                {"status", Tag::Status}};

const Parser::TagOrderMap Parser::_propfind_tag_order{{Tag::None,             {Tag::Multistatus}},
                                                      {Tag::Multistatus,      {Tag::Response}},
                                                      {Tag::Response,         {Tag::Href, Tag::PropStat}},
                                                      {Tag::PropStat,         {Tag::Prop, Tag::Status}},
                                                      {Tag::Prop,             {Tag::CreationDate, Tag::GetLastModified, Tag::ResourceType, Tag::GetContentLength}},
                                                      {Tag::ResourceType,     {Tag::Collection}},
                                                      {Tag::Href,             {}},
                                                      {Tag::CreationDate,     {}},
                                                      {Tag::GetLastModified,  {}},
                                                      {Tag::Collection,       {}},
                                                      {Tag::GetContentLength, {}},
                                                      {Tag::Status,           {}}};

Parser::Parser(const std::filesystem::path& current_path) : _state(current_path, _result) {
    assert(_propfind_tag_order.find(Tag::None) != std::cend(_propfind_tag_order));
    assert(_propfind_tag_by_str_map.size() + 1 == _propfind_tag_order.size());
}

std::filesystem::path Parser::get_current_path() const { return _state.current_path; }

bool Parser::has_error() const {
    _response_text_stream << '\0';
#ifdef ANDROID
    const std::string response_text_view = _response_text_stream.str();
#else
    const std::string_view response_text_view = _response_text_stream.view();
#endif
    if (_reader.hasError() || !_critical_error_text.empty()) {
        const std::string text = _reader.hasError() ? "invalid XML format" : _critical_error_text;
        qCritical(qUtf8Printable(QObject::tr("Response parse error: %s, path: %s. Response text:\n%s")), qUtf8Printable(QString::fromStdString(text)), qUtf8Printable(QString::fromStdString(_state.current_path.generic_string())), response_text_view.data());
        _response_text_stream.str(std::string());
        return true;
    }
    if (_state.has_error)
        qWarning(qUtf8Printable(QObject::tr("Response text (path: %s):\n%s")), qUtf8Printable(QString::fromStdString(_state.current_path.generic_string())), response_text_view.data());
    else
        qDebug(qUtf8Printable(QObject::tr("Response text (path: %s):\n%s")), qUtf8Printable(QString::fromStdString(_state.current_path.generic_string())), response_text_view.data());

    _response_text_stream.str(std::string());
    return false;
}

Parser::Result&& Parser::get_result() { return std::move(_result); }

void Parser::parse_response_portion(const ReadBuffer& data) {
    _response_text_stream << data.data();
    if (!_critical_error_text.empty())
        return;

    _reader.addData(data.data());
    try {
        while (!_reader.atEnd())
            handle_token(_reader.readNext());
    } catch (const Exception& e) {
        _critical_error_text = e.what();
    }
}

void Parser::handle_token(QXmlStreamReader::TokenType token) {
    switch (token) {
        case QXmlStreamReader::StartElement: {
            QStringView& not_dav = _state.not_dav_namespace;
            if (!not_dav.isNull())
                return;

            const QStringView namespace_uri = _reader.namespaceUri();
            if (namespace_uri != QStringLiteral("DAV:")) {
                not_dav = _reader.name();
                return;
            }
            const auto tag_it = _propfind_tag_by_str_map.find(_reader.name().toString());
            const auto not_found = tag_it == std::end(_propfind_tag_by_str_map);
            if (not_found)
                throw Exception("unknown tag");

            const Tag tag = tag_it->second;
            const TagSet& possible_tags = _state.stack.top()->second;
            if (!possible_tags.contains(tag))
                throw Exception("incorrect tag order");

            _state.process_start_of_tag(tag);
            const auto possible_tags_it = _propfind_tag_order.find(tag);
            assert(possible_tags_it != std::end(_propfind_tag_order));
            _state.stack.push(possible_tags_it);
            break;
        }

        case QXmlStreamReader::Characters: {
            if (!_state.not_dav_namespace.isNull() || _reader.isCDATA())
                return;

            const Tag tag = _state.stack.top()->first;
            _state.process_data(tag, _reader.text());
            break;
        }

        case QXmlStreamReader::EndElement: {
            QStringView& not_dav = _state.not_dav_namespace;
            if (!not_dav.isNull()) {
                if (not_dav == _reader.name())
                    not_dav = QStringView();

                return;
            }
            const Tag tag = _state.stack.top()->first;
            _state.process_end_of_tag(tag);
            _state.stack.pop();
            break;
        }

        default:
            break;
    }
}
