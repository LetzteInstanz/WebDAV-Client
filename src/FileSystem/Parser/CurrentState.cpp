#include "CurrentState.h"

#include "TimeParser.h"

Parser::CurrentState::CurrentState(const QStringView& current_path, TagOrderMap::const_iterator first, Result& result) : _current_path(current_path), _result(result) { stack.push(first); }

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
            _obj.replace_unknown_status(_status);
            _status = FSObjectStruct::Status::None;
            break;
        }

        case Tag::Response: {
            if (_obj.type.first != FileSystemObject::Status::Ok) {
                if (_obj.is_curr_dir_obj)
                    throw std::runtime_error("the resourcetype property of the current directory object hasn't ok status");

                set_error(QObject::tr("a resourcetype property hasn't ok status"));
                break;
            }
            if (_obj.name.isEmpty()) {
                if (_obj.is_curr_dir_obj)
                    throw std::runtime_error("the href tag data of the current directory object is empty");

                set_error(QObject::tr("a href tag data is empty"));
                break;
            }
            FileSystemObject obj(std::move(_obj.name), _obj.type.second, std::move(_obj.creation_date), std::move(_obj.last_modified), std::move(_obj.content_length));
            if (_obj.is_curr_dir_obj)
                _result.first = std::make_unique<FileSystemObject>(std::move(obj));
            else
                _result.second.emplace_back(std::move(obj));

            break;
        }

        default:
            break;
    }
}

void Parser::CurrentState::update_if_data(Tag t, const QStringView& data) {
    switch (t) {
        case Tag::Href: {
            QString abs_path = QUrl::fromPercentEncoding(data.toLatin1());
            if (abs_path.back() != '/')
                abs_path += '/';

            _obj.is_curr_dir_obj = abs_path == _current_path;
            _obj.name = FSObjectStruct::extract_name(abs_path);
            break;
        }

        case Tag::CreationDate: {
            try {
                _obj.creation_date = std::make_pair(std::remove_reference_t<FSObjectStruct::Status>(_status), TimeParser::to_sys_seconds(data, TimeParser::Format::Rfc3339));
            } catch (const std::runtime_error& e) {
                _obj.creation_date.first = FSObjectStruct::Status::None;
                set_error(QObject::tr(e.what()));
            }
            break;
        }

        case Tag::GetLastModified: {
            try {
                _obj.last_modified = std::make_pair(std::remove_reference_t<FSObjectStruct::Status>(_status), TimeParser::to_sys_seconds(data, TimeParser::Format::Rfc2616));
            } catch (const std::runtime_error& e) {
                _obj.last_modified.first = FSObjectStruct::Status::None;
                set_error(QObject::tr(e.what()));
            }
            break;
        }

        case Tag::GetContentLength: {
            bool ok;
            _obj.content_length.second = data.toULongLong(&ok);
            if (!ok) {
                _obj.content_length.first = FSObjectStruct::Status::None;
                set_error(QObject::tr("getcontentlength property isn't a number"));
            }
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
