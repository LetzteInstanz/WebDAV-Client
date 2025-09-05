#include "ParserTest.h"

#include "../FileSystem/FileSystemObject.h"
#include "../Util.h"
#include "Util.h"

using namespace std::chrono_literals;

namespace {
    void pass_response(Parser& parser, const char* text) {
        auto length = std::strlen(text);
        ReadBuffer buffer;
        const auto size = buffer.size() - 1; // note: the last is for null-terminator
        for (std::size_t i = 0; length != 0; i += size) {
            const auto& n = std::min(size, length);
            std::memcpy(buffer.data(), text + i, n);
            buffer[n] = 0;
            parser.parse_response_portion(buffer);
            length = length < size ? 0 : length - size;
        }
    }

    void test_date_time(const std::optional<std::chrono::sys_seconds>& time, std::chrono::hours h, std::chrono::minutes min, std::chrono::seconds s, std::chrono::day d, std::chrono::month m, std::chrono::year y) {
        if (!time)
            return;

        const auto yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(*time));
        print_stack_if_false(yyyy_mm_dd.year() == y);
        print_stack_if_false(yyyy_mm_dd.month() == m);
        print_stack_if_false(yyyy_mm_dd.day() == d);
        const auto hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(*time - to_type<std::chrono::sys_days>(yyyy_mm_dd));
        print_stack_if_false(hh_mm_ss.hours() == h);
        print_stack_if_false(hh_mm_ss.minutes() == min);
        print_stack_if_false(hh_mm_ss.seconds() == s);
    }

    void check_current_dir(const Parser::CurrDirObj& current_dir) {
        std::print("Testing current directory…\n");
        print_stack_if_false(current_dir.has_value());
        print_stack_if_false(current_dir->get_name() == "dav");
        print_stack_if_false(current_dir->get_type() == FileSystemObject::Type::Directory);
        print_stack_if_false(!current_dir->get_creation_time());
        print_stack_if_false(!current_dir->get_modification_time());
        print_stack_if_false(!current_dir->get_size());
    }

    void check_dir1(Parser::Objects::const_iterator it) {
        std::print("Testing directory 1…\n");
        print_stack_if_false(it->get_name() == "Диск 1");
        print_stack_if_false(it->get_type() == FileSystemObject::Type::Directory);

        std::optional<std::chrono::sys_seconds> time = it->get_creation_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 23h, 20min, 51s, 12d, std::chrono::April, 1985y);

        time = it->get_modification_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 8h, 49min, 37s, 6d, std::chrono::November, 1999y);

        print_stack_if_false(!it->get_size());
    }

    void check_dir2(Parser::Objects::const_iterator it) {
        std::print("Testing directory 2…\n");
        print_stack_if_false(it->get_name() == "Диск 2");
        print_stack_if_false(it->get_type() == FileSystemObject::Type::Directory);

        std::optional<std::chrono::sys_seconds> time = it->get_creation_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 17h, 56min, 57s, 18d, std::chrono::December, 1996y);

        time = it->get_modification_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 9h, 50min, 38s, 7d, std::chrono::October, 2025y);

        print_stack_if_false(!it->get_size());
    }

    void check_file1(Parser::Objects::const_iterator it) {
        std::print("Testing file 1…\n");
        print_stack_if_false(it->get_name() == "Тестовый файл.txt");
        print_stack_if_false(it->get_type() == FileSystemObject::Type::File);
        print_stack_if_false(!it->get_creation_time());

        std::optional<std::chrono::sys_seconds> time = it->get_modification_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 6h, 51min, 39s, 8d, std::chrono::January, 2000y);

        std::optional<uint64_t> size = it->get_size();
        print_stack_if_false(size.has_value());
        print_stack_if_false(*size == 1743607603214301);
    }

    void check_file2(Parser::Objects::const_iterator it) {
        std::print("Testing file 2…\n");
        print_stack_if_false(it->get_name() == "Тестовый файл 2.txt");
        print_stack_if_false(it->get_type() == FileSystemObject::Type::File);

        std::optional<std::chrono::sys_seconds> time = it->get_creation_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 3h, 17min, 55s, 15d, std::chrono::July, 1997y);

        time = it->get_modification_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 19h, 56min, 7s, 8d, std::chrono::January, 2069y);

        print_stack_if_false(!it->get_size());
    }

    void check_file3(Parser::Objects::const_iterator it) {
        std::print("Testing file 3…\n");
        print_stack_if_false(it->get_name() == "Тестовый файл 3.txt");
        print_stack_if_false(it->get_type() == FileSystemObject::Type::File);

        std::optional<std::chrono::sys_seconds> time = it->get_creation_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 11h, 15min, 43s, 18d, std::chrono::March, 2025y);

        time = it->get_modification_time();
        print_stack_if_false(time.has_value());
        test_date_time(time, 19h, 56min, 7s, 8d, std::chrono::January, 1970y);

        print_stack_if_false(!it->get_size());
    }
}

void ParserTest::run() {
    std::print("Testing parser…\n");
    test_correct_response();
    test_resourcetype_has_non_ok_status();
    test_resourcetype_is_absent();
    test_current_dir_without_collection();
    test_empty_href();
    test_href_is_absent();
    test_status_is_absent();
    test_empty_status();
    test_unknown_tag();
    test_incorrect_tag_order();
}

void ParserTest::test_correct_response() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:response>\n" // note: current directory "/dav"
        "<D:href>/dav</D:href>\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:resourcetype><D:collection/></D:resourcetype>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 403 Forbidden</D:status>\n"
            "<D:prop>\n"
                "<D:creationdate />\n"
                "<D:getlastmodified />\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>"
            "<D:prop>"
                "<D:getcontentlength/>"
            "</D:prop>\n"
        "</D:propstat>\n"
    "</D:response>\n"

    "<not_dav_namespace />"
    "<N:multistatus xmlns:N=\"kek:\" />\n"

    "<D:response>" // note: directory "Диск 1"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<D:getcontentlength/>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
            "<D:prop>\n"
                "<D:getlastmodified>Sat, 06 Nov 1999 08:49:37 GMT</D:getlastmodified>\n" // note: the date and time format is tested (https://datatracker.ietf.org/doc/html/rfc2518#section-13.7)
                "<D:resourcetype><D:collection/></D:resourcetype>\n"
                "<D:creationdate>1985-04-12T23:20:50.554654956Z</D:creationdate>\n" // note: the date and time format is tested (https://datatracker.ietf.org/doc/html/rfc4918#section-15.1)
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%d0%94%d0%b8%d1%81%d0%ba%201</D:href>\n" // note: a directory path without the slash at the end is tested
    "</D:response>\n"

    "<D:response>\n" // note: directory "Диск 2"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:creationdate>1996-12-19T02:29:57+08:33</D:creationdate>\n" // note: the date and time format is tested
                "<D:getlastmodified>Tue Oct 7 09:50:38 2025</D:getlastmodified>\n" // note: the date and time format is tested
                "<D:resourcetype><D:collection/></D:resourcetype>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>"
            "<D:prop>\n"
                "<D:getcontentlength/>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%d0%94%d0%b8%d1%81%d0%ba%202/</D:href>\n"
    "</D:response>\n"

    "<D:response>\n" // note: file "Тестовый файл.txt"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:getcontentlength>1743607603214301</D:getcontentlength>\n"
                "<D:getlastmodified>Saturday, 08-Jan-00 06:51:39 GMT</D:getlastmodified>\n" // note: the year format YY with the year greater than 1999 is tested
                "<D:resourcetype />\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<D:creationdate />\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%D0%A2%D0%B5%D1%81%D1%82%D0%BE%D0%B2%D1%8B%D0%B9%20%D1%84%D0%B0%D0%B9%D0%BB.txt/</D:href>\n" // note: a file path with the slash at the end is tested
    "</D:response>\n"

    "<D:response>\n" // note: file "Тестовый файл 2.txt"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:getlastmodified>Wednesday, 08-Jan-69 19:56:07 GMT</D:getlastmodified>\n" // note: the year format YY with the year greater than 1999 is tested
                "<D:creationdate>1997-07-14T21:37:55.4756743-05:40</D:creationdate>\n" // note: the date and time format is tested
                "<D:resourcetype />\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%D0%A2%D0%B5%D1%81%D1%82%D0%BE%D0%B2%D1%8B%D0%B9%20%D1%84%D0%B0%D0%B9%D0%BB%202.txt</D:href>\n"
    "</D:response>\n"

    "<D:response>\n" // note: file "Тестовый файл 3.txt"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:creationdate>2025-03-18T11:15:43Z</D:creationdate>\n" // note: the date and time format is tested
                "<D:resourcetype />\n"
                "<D:getlastmodified>Thursday, 08-Jan-70 19:56:07 GMT</D:getlastmodified>\n" // note: the year format YY with the year less than 1999 is tested
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%D0%A2%D0%B5%D1%81%D1%82%D0%BE%D0%B2%D1%8B%D0%B9%20%D1%84%D0%B0%D0%B9%D0%BB%203.txt</D:href>\n"
    "</D:response>\n"
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/dav/"));
    pass_response(_parser, response);
    try {
        const Parser::Result result = _parser.get_result();
        print_stack_if_false(result.second.size() == 5);
        check_current_dir(result.first);
        auto it = std::begin(result.second);
        check_dir1(it);
        ++it;
        check_dir2(it);
        ++it;
        check_file1(it);
        ++it;
        check_file2(it);
        ++it;
        check_file3(it);
    } catch (const Parser::Exception&) { print_stack_if_false(false); }
    _parser.reset();
}

void ParserTest::test_resourcetype_has_non_ok_status() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:response>\n"
        "<D:href>/current_dir</D:href>\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:resourcetype><D:collection/></D:resourcetype>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
    "</D:response>\n"

    "<D:response>\n"
        "<D:href>/current_dir/file</D:href>\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:resourcetype />\n" // note: resourcetype property must have ok status
            "</D:prop>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
        "</D:propstat>\n"
    "</D:response>\n"
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}

void ParserTest::test_resourcetype_is_absent() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:response>\n"
        "<D:href>/current_dir</D:href>\n"
        "<D:propstat>\n"
            "<D:prop>\n" // note: there isn't resourcetype property
                "<D:getlastmodified>Sat, 06 Nov 1999 08:49:37 GMT</D:getlastmodified>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
    "</D:response>\n"
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}

void ParserTest::test_current_dir_without_collection() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:response>\n"
        "<D:href>/current_dir</D:href>\n"
        "<D:propstat>\n"
            "<D:prop>\n" // note: current directory must have collection XML element
                "<D:resourcetype/>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
    "</D:response>\n"
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}

void ParserTest::test_empty_href() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:response>\n"
        "<D:href></D:href>\n" // note: empty href XML element's data
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:resourcetype><D:collection/></D:resourcetype>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
    "</D:response>\n"
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}

void ParserTest::test_href_is_absent() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:response>\n"
        "<D:propstat>\n" // note: there isn't href XML element
            "<D:prop>\n"
                "<D:resourcetype><D:collection/></D:resourcetype>\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
    "</D:response>\n"
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}

void ParserTest::test_status_is_absent() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:response>\n"
        "<D:href>/current_dir</D:href>\n"
        "<D:propstat>\n" // note: there isn't status XML element
            "<D:prop>\n"
                "<D:resourcetype><D:collection/></D:resourcetype>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
    "</D:response>\n"
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}

void ParserTest::test_empty_status() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:response>\n"
        "<D:href>/current_dir</D:href>\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<D:resourcetype><D:collection/></D:resourcetype>\n"
            "</D:prop>\n"
            "<D:status></D:status>\n" // note: empty status XML element's data
        "</D:propstat>\n"
    "</D:response>\n"
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}

void ParserTest::test_unknown_tag() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\">\n"
    "<D:kek />\n" // note: unknown XML element
"</D:multistatus>";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}

void ParserTest::test_incorrect_tag_order() {
    const char* response =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:response xmlns:D=\"DAV:\">\n" // note: response XML element must be inside multistatus element
    "<D:multistatus/>\n"
"</D:response>\n";
    _parser.set_current_path(std::filesystem::path("/current_dir/"));
    pass_response(_parser, response);
    print_stack_if_no_exception<Parser::Exception>([this]() { _parser.get_result(); });
    _parser.reset();
}
