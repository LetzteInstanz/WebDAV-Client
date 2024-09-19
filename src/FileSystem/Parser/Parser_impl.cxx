module;

#include <cassert>
#include <stdexcept>

#include <QObject>
#include <QStringLiteral>
#include <QXmlStreamReader>
#include <QtGlobal>

#include "../../Util.h"

#ifndef NDEBUG
#include "../FileSystemObject.h"
#endif

module Parser;

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

Parser::Result Parser::parse_propfind_reply(const QStringView& current_path, const QByteArray& data) {
    assert(!current_path.empty());
    assert(current_path.back() == '/');
    assert(_propfind_tag_by_str_map.size() + 1 == _propfind_tag_order.size());

    Result result;
    const auto first = _propfind_tag_order.find(Tag::None);
    assert(first != std::end(_propfind_tag_order));
    CurrentState state(current_path, first, result);
    QXmlStreamReader reader(data);
    while (!reader.atEnd()) {
        switch (reader.readNext()) {
            case QXmlStreamReader::StartElement: {
                QStringView& not_dav = state.not_dav_namespace;
                if (!not_dav.isNull())
                    continue;

                const QStringView namespace_uri = reader.namespaceUri();
                if (namespace_uri != QStringLiteral("DAV:")) {
                    not_dav = reader.name();
                    continue;
                }
                const auto tag_it = _propfind_tag_by_str_map.find(reader.name().toString());
                const auto not_found = tag_it == std::end(_propfind_tag_by_str_map);
                if (not_found)
                    throw std::runtime_error("unknown tag");

                const Tag tag = tag_it->second;
                const TagSet& possible_tags = state.stack.top()->second;
                if (!possible_tags.contains(tag))
                    throw std::runtime_error("incorrect tag order");

                state.update_if_start_tag(tag);
                const auto possible_tags_it = _propfind_tag_order.find(tag);
                assert(possible_tags_it != std::end(_propfind_tag_order));
                state.stack.push(possible_tags_it);
                break;
            }

            case QXmlStreamReader::EndElement: {
                QStringView& not_dav = state.not_dav_namespace;
                if (!not_dav.isNull()) {
                    if (not_dav == reader.name())
                        not_dav = QStringView();

                    continue;
                }
                const Tag tag = state.stack.top()->first;
                state.update_if_end_tag(tag);
                state.stack.pop();
                break;
            }

            case QXmlStreamReader::Characters: {
                if (!state.not_dav_namespace.isNull() || reader.isCDATA())
                    continue;

                const Tag tag = state.stack.top()->first;
                state.update_if_data(tag, reader.text());
                break;
            }

            default:
                break;
        }
    }
    if (reader.hasError())
        throw std::runtime_error("invalid XML format");

    if (state.was_error)
        qWarning(qUtf8Printable(QObject::tr("The reply text: \n%s")), qUtf8Printable(data));
    else
        qDebug(qUtf8Printable(QObject::tr("The reply text: \n%s")), qUtf8Printable(data));

    return result;
}

#ifndef NDEBUG
void Parser::test() {
    using namespace std::chrono_literals;

    const QString test_responce =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\" xmlns:ns0=\"DAV:\">\n"
    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n"
        "<D:href>/dav</D:href>\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:resourcetype><D:collection/></lp1:resourcetype>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 403 Forbidden</D:status>\n"
            "<D:prop>\n"
                "<lp1:creationdate />\n"
                "<lp1:getlastmodified />\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<lp1:getcontentlength/>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<lp1:getcontentlength/>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
            "<D:prop>\n"
                "<lp1:getlastmodified>Mon, 06 Mar 2023 13:49:01 GMT</lp1:getlastmodified>\n"
                "<lp1:resourcetype><D:collection/></lp1:resourcetype>\n"
                "<lp1:creationdate>1996-12-19T16:29:57+08:30</lp1:creationdate>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%d0%94%d0%b8%d1%81%d0%ba%201</D:href>\n" // Диск 1
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:creationdate>1996-12-19T16:39:57.473-08:21</lp1:creationdate>\n"
                "<lp1:getlastmodified>Thu, 09 Mar 2023 06:55:56 GMT</lp1:getlastmodified>\n"
                "<lp1:resourcetype><D:collection/></lp1:resourcetype>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<lp1:getcontentlength/>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%d0%94%d0%b8%d1%81%d0%ba%202/</D:href>\n" // Диск 2
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
            "<D:prop>\n"
                "<lp1:getlastmodified>Wednesday, 16-Jul-2025 23:59:58 GMT</lp1:getlastmodified>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%d0%94%d0%b8%d1%81%d0%ba%203/</D:href>\n" // Диск 3
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<lp1:getcontentlength/>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:resourcetype />\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 401 Unauthorized</D:status>\n"
        "</D:propstat>\n"
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:getcontentlength>1743607603214300</lp1:getcontentlength>\n"
                "<lp1:getlastmodified>Wednesday, 16-Jul-2025 23:59:58 GMT</lp1:getlastmodified>\n"
                "<lp1:resourcetype />\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<lp1:creationdate />\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%D0%A2%D0%B5%D1%81%D1%82%D0%BE%D0%B2%D1%8B%D0%B9%20%D1%84%D0%B0%D0%B9%D0%BB.txt</D:href>\n" // Тестовый файл.txt
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:getlastmodified>Wednesday, 16-Jul-2025 23:59:58 GMT</lp1:getlastmodified>\n"
                "<lp1:resourcetype />\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<lp1:creationdate />\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href></D:href>\n"
    "</D:response>\n"
"</D:multistatus>";
    const Result result = parse_propfind_reply(QString("/dav/"), test_responce.toLatin1());
    const FileSystemObject* const obj = result.first.get();
    assert(obj);
    assert(obj->get_name() == "dav");
    assert(obj->get_type() == FileSystemObject::Type::Directory);
    assert(obj->is_creation_time_valid() == false);
    assert(obj->is_modification_time_valid() == false);
    assert(obj->is_size_valid() == false);

    assert(result.second.size() == 3);

    auto it = std::begin(result.second);
    assert(it->get_name() == "Диск 1");
    assert(it->get_type() == FileSystemObject::Type::Directory);
    assert(it->is_creation_time_valid());
    std::chrono::sys_seconds seconds = it->get_creation_time();
    std::chrono::year_month_day yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 1996y);
    assert(yyyy_mm_dd.month() == std::chrono::December);
    assert(yyyy_mm_dd.day() == 19d);
    std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration> hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
    assert(hh_mm_ss.hours() == 7h);
    assert(hh_mm_ss.minutes() == 59min);
    assert(hh_mm_ss.seconds() == 57s);
    assert(it->is_modification_time_valid());
    seconds = it->get_modification_time();

    yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 2023y);
    assert(yyyy_mm_dd.month() == std::chrono::March);
    assert(yyyy_mm_dd.day() == 6d);
    hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
    assert(hh_mm_ss.hours() == 13h);
    assert(hh_mm_ss.minutes() == 49min);
    assert(hh_mm_ss.seconds() == 1s);
    assert(it->is_size_valid() == false);

    ++it;
    assert(it->get_name() == "Диск 2");
    assert(it->get_type() == FileSystemObject::Type::Directory);
    assert(it->is_creation_time_valid());
    seconds = it->get_creation_time();

    yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 1996y);
    assert(yyyy_mm_dd.month() == std::chrono::December);
    assert(yyyy_mm_dd.day() == 20d);
    hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
    assert(hh_mm_ss.hours() == 1h);
    assert(hh_mm_ss.minutes() == 0min);
    assert(hh_mm_ss.seconds() == 57s);
    assert(it->is_modification_time_valid());
    seconds = it->get_modification_time();

    yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 2023y);
    assert(yyyy_mm_dd.month() == std::chrono::March);
    assert(yyyy_mm_dd.day() == 9d);
    hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
    assert(hh_mm_ss.hours() == 6h);
    assert(hh_mm_ss.minutes() == 55min);
    assert(hh_mm_ss.seconds() == 56s);
    assert(it->is_size_valid() == false);

    ++it;
    assert(it->get_name() == "Тестовый файл.txt");
    assert(it->get_type() == FileSystemObject::Type::File);
    assert(it->is_creation_time_valid() == false);
    assert(it->is_modification_time_valid());
    seconds = it->get_modification_time();

    yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 2025y);
    assert(yyyy_mm_dd.month() == std::chrono::July);
    assert(yyyy_mm_dd.day() == 16d);
    hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
    assert(hh_mm_ss.hours() == 23h);
    assert(hh_mm_ss.minutes() == 59min);
    assert(hh_mm_ss.seconds() == 58s);
    assert(it->is_size_valid());
    assert(it->get_size() == 1743607603214300);
}
#endif
