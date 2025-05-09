#include "Parser.h"

#include "CurrentState.h"

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

Parser::Result Parser::parse_propfind_reply(QStringView current_path, const QByteArray& data) {
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
