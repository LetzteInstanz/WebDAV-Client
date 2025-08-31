#include "CurrentState.h"

#include "TimeParser.h"

Parser::CurrentState::CurrentState(const std::filesystem::path& current_path, TagOrderMap::const_iterator first, Result& result) : _current_path(current_path), _result(result) {
    stack.push(first);
}

void Parser::CurrentState::update_if_start_tag(Tag t) {
    switch (t) {
        case Tag::Response:
            _obj = {};
            break;

        case Tag::PropStat:
            _status = FSObjectStruct::Status::Unknown;
            break;

        case Tag::ResourceType:
            _obj.type.first = _status;
            break;

        case Tag::CreationDate:
            _obj.creation_date.first = _status;
            break;

        case Tag::GetLastModified:
            _obj.last_modified.first = _status;
            break;

        case Tag::Collection:
            _obj.type = std::make_pair(_status, FileSystemObject::Type::Directory);
            break;

        case Tag::GetContentLength:
            _obj.content_length.first = _status;
            break;

        default:
            break;
    }
}

void Parser::CurrentState::update_if_end_tag(Tag t) {
    switch (t) {
        case Tag::PropStat: {
            if (!_status.has_value())
                throw std::runtime_error("the propstat element hasn't the status element");

            _obj.replace_unknown_status(_status);
            _status.reset();
            break;
        }

        case Tag::Response: {
            const auto is_dir = _obj.type.second == FSObjectStruct::Type::Directory;
            if (is_dir && _obj.path.has_filename())
                _obj.path /= std::filesystem::path();

            if (!is_dir && !_obj.path.has_filename())
                _obj.path = _obj.path.parent_path();

            _obj.is_curr_dir_obj = _obj.path == _current_path;
            if (_obj.type.first != FSObjectStruct::Status::Ok) {
                if (_obj.is_curr_dir_obj)
                    throw std::runtime_error("the resourcetype property of the current directory object hasn't ok status");

                set_error(QObject::tr("a resourcetype property hasn't ok status"));
                break;
            }
            if (_obj.path.empty()) {
                set_error(QObject::tr("a href tag data is empty"));
                break;
            }
            FileSystemObject obj(std::move(_obj.path), std::move(_obj.creation_date.second), std::move(_obj.last_modified.second), std::move(_obj.content_length.second));
            if (_obj.is_curr_dir_obj)
                _result.first = std::move(obj);
            else
                _result.second.emplace_back(std::move(obj));

            break;
        }

        default:
            break;
    }
}

void Parser::CurrentState::update_if_data(Tag t, QStringView data) {
    switch (t) {
        case Tag::Href: {
            _obj.path = std::filesystem::path(QUrl::fromPercentEncoding(data.toLatin1()).toStdString());
            break;
        }

        case Tag::CreationDate: {
            try {
                _obj.creation_date = std::make_pair(_status, TimeParser::to_sys_seconds(data, TimeParser::Format::Rfc3339));
            } catch (const std::runtime_error& e) {
                set_error(QObject::tr(e.what()));
            }
            break;
        }

        case Tag::GetLastModified: {
            try {
                _obj.last_modified = std::make_pair(_status, TimeParser::to_sys_seconds(data, TimeParser::Format::Rfc2068));
            } catch (const std::runtime_error& e) {
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
            _status = FSObjectStruct::to_status(data);
            break;
        }

        default:
            break;
    }
}

void Parser::CurrentState::set_error(QString&& msg) {
    was_error = true;
    qWarning(qUtf8Printable(QObject::tr("An error has occured during the reply parse: %s.")), qUtf8Printable(msg));
}
