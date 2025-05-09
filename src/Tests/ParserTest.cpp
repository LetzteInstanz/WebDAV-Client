#include "ParserTest.h"

#include "../FileSystem/FileSystemObject.h"
#include "Util.h"

using namespace std::chrono_literals;

namespace {
    void test_date_time(std::chrono::sys_seconds time, std::chrono::hours h, std::chrono::minutes min, std::chrono::seconds s, std::chrono::day d, std::chrono::month m, std::chrono::year y) {
        const auto yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(time));
        print_stack_if_false(yyyy_mm_dd.year() == y);
        print_stack_if_false(yyyy_mm_dd.month() == m);
        print_stack_if_false(yyyy_mm_dd.day() == d);
        const auto hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(time - to_type<std::chrono::sys_days>(yyyy_mm_dd));
        print_stack_if_false(hh_mm_ss.hours() == h);
        print_stack_if_false(hh_mm_ss.minutes() == min);
        print_stack_if_false(hh_mm_ss.seconds() == s);
    }
}

void ParserTest::run() {
    std::print("Testing parser…\n");
    const QString test_responce =
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
"<D:multistatus xmlns:D=\"DAV:\" xmlns:ns0=\"DAV:\">\n"
    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n" // note: current directory "/dav"
        "<D:href>/dav/</D:href>\n"
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

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n" // note: directory "Диск 1"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<lp1:getcontentlength/>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
            "<D:prop>\n"
                "<lp1:getlastmodified>Sat, 06 Nov 1999 08:49:37 GMT</lp1:getlastmodified>\n" // note: the date and time format is tested (https://www.rfc-editor.org/rfc/rfc2518#section-13.7)
                "<lp1:resourcetype><D:collection/></lp1:resourcetype>\n"
                "<lp1:creationdate>1985-04-12T23:20:50.554654956Z</lp1:creationdate>\n" // note: the date and time format is tested (https://www.rfc-editor.org/rfc/rfc4918#section-15)
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%d0%94%d0%b8%d1%81%d0%ba%201</D:href>\n"
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n" // note: directory "Диск 2"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:creationdate>1996-12-19T02:29:57+08:33</lp1:creationdate>\n" // note: the date and time format is tested
                "<lp1:getlastmodified>Tue Oct 7 09:50:38 2025</lp1:getlastmodified>\n" // note: the date and time format is tested
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
        "<D:href>/dav/%d0%94%d0%b8%d1%81%d0%ba%202/</D:href>\n"
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n" // note: directory "Диск 3" (invalid response)
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
            "<D:prop>\n"
                "<lp1:getlastmodified>Saturday, 05-Dec-98 07:48:36 GMT</lp1:getlastmodified>\n" // note: the date and time format is tested
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%d0%94%d0%b8%d1%81%d0%ba%203/</D:href>\n"
        "<D:propstat>\n"
            "<D:status>HTTP/1.1 404 Not Found</D:status>\n"
            "<D:prop>\n"
                "<lp1:getcontentlength/>\n"
            "</D:prop>\n"
        "</D:propstat>\n"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:resourcetype />\n" // note: invalid data
            "</D:prop>\n"
            "<D:status>HTTP/1.1 401 Unauthorized</D:status>\n"
        "</D:propstat>\n"
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n" // note: file "Тестовый файл.txt"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:getcontentlength>1743607603214301</lp1:getcontentlength>\n"
                "<lp1:getlastmodified>Saturday, 08-Jan-00 06:51:39 GMT</lp1:getlastmodified>\n" // note: the year format YY with the year greater than 1999 is tested
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
        "<D:href>/dav/%D0%A2%D0%B5%D1%81%D1%82%D0%BE%D0%B2%D1%8B%D0%B9%20%D1%84%D0%B0%D0%B9%D0%BB.txt</D:href>\n"
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n" // note: file with the empty href-tag (invalid response)
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:getlastmodified>Saturday, 08-Jan-00 06:51:39 GMT</lp1:getlastmodified>\n"
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
        "<D:href></D:href>\n" // note: invalid data
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n" // note: file "Тестовый файл 2.txt"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:getlastmodified>Wednesday, 08-Jan-69 19:56:07 GMT</lp1:getlastmodified>\n" // note: the year format YY with the year greater than 1999 is tested
                "<lp1:creationdate>1997-07-14T21:37:55.4756743-05:40</lp1:creationdate>\n" // note: the date and time format is tested
                "<lp1:resourcetype />\n"
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%D0%A2%D0%B5%D1%81%D1%82%D0%BE%D0%B2%D1%8B%D0%B9%20%D1%84%D0%B0%D0%B9%D0%BB%202.txt</D:href>\n"
    "</D:response>\n"

    "<D:response xmlns:lp1=\"DAV:\" xmlns:lp2=\"http://apache.org/dav/props/\" xmlns:g0=\"DAV:\">\n" // note: file "Тестовый файл 3.txt"
        "<D:propstat>\n"
            "<D:prop>\n"
                "<lp1:creationdate>2025-03-18T11:15:43Z</lp1:creationdate>\n" // note: the date and time format is tested
                "<lp1:resourcetype />\n"
                "<lp1:getlastmodified>Thursday, 08-Jan-70 19:56:07 GMT</lp1:getlastmodified>\n" // note: the year format YY with the year less than 1999 is tested
            "</D:prop>\n"
            "<D:status>HTTP/1.1 200 OK</D:status>\n"
        "</D:propstat>\n"
        "<D:href>/dav/%D0%A2%D0%B5%D1%81%D1%82%D0%BE%D0%B2%D1%8B%D0%B9%20%D1%84%D0%B0%D0%B9%D0%BB%203.txt</D:href>\n"
    "</D:response>\n"
"</D:multistatus>";
    const Parser::Result result = Parser::parse_propfind_reply(QString("/dav/"), test_responce.toLatin1());
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
}

void ParserTest::check_current_dir(const Parser::CurrDirObj& current_dir) {
    std::print("Testing current directory…\n");
    print_stack_if_false(current_dir != nullptr);
    print_stack_if_false(current_dir->get_name() == "dav");
    print_stack_if_false(current_dir->get_type() == FileSystemObject::Type::Directory);
    print_stack_if_false(!current_dir->is_creation_time_valid());
    print_stack_if_false(!current_dir->is_modification_time_valid());
    print_stack_if_false(!current_dir->is_size_valid());
}

void ParserTest::check_dir1(Parser::Objects::const_iterator it) {
    std::print("Testing directory 1…\n");
    print_stack_if_false(it->get_name() == "Диск 1");
    print_stack_if_false(it->get_type() == FileSystemObject::Type::Directory);

    print_stack_if_false(it->is_creation_time_valid());
    test_date_time(it->get_creation_time(), 23h, 20min, 51s, 12d, std::chrono::April, 1985y);

    print_stack_if_false(it->is_modification_time_valid());
    test_date_time(it->get_modification_time(), 8h, 49min, 37s, 6d, std::chrono::November, 1999y);

    print_stack_if_false(!it->is_size_valid());
}

void ParserTest::check_dir2(Parser::Objects::const_iterator it) {
    std::print("Testing directory 2…\n");
    print_stack_if_false(it->get_name() == "Диск 2");
    print_stack_if_false(it->get_type() == FileSystemObject::Type::Directory);

    print_stack_if_false(it->is_creation_time_valid());
    test_date_time(it->get_creation_time(), 17h, 56min, 57s, 18d, std::chrono::December, 1996y);

    print_stack_if_false(it->is_modification_time_valid());
    test_date_time(it->get_modification_time(), 9h, 50min, 38s, 7d, std::chrono::October, 2025y);

    print_stack_if_false(!it->is_size_valid());
}

void ParserTest::check_file1(Parser::Objects::const_iterator it) {
    std::print("Testing file 1…\n");
    print_stack_if_false(it->get_name() == "Тестовый файл.txt");
    print_stack_if_false(it->get_type() == FileSystemObject::Type::File);
    print_stack_if_false(it->is_creation_time_valid() == false);

    print_stack_if_false(it->is_modification_time_valid());
    test_date_time(it->get_modification_time(), 6h, 51min, 39s, 8d, std::chrono::January, 2000y);

    print_stack_if_false(it->is_size_valid());
    print_stack_if_false(it->get_size() == 1743607603214301);
}

void ParserTest::check_file2(Parser::Objects::const_iterator it) {
    std::print("Testing file 2…\n");
    print_stack_if_false(it->get_name() == "Тестовый файл 2.txt");
    print_stack_if_false(it->get_type() == FileSystemObject::Type::File);

    print_stack_if_false(it->is_creation_time_valid());
    test_date_time(it->get_creation_time(), 3h, 17min, 55s, 15d, std::chrono::July, 1997y);

    print_stack_if_false(it->is_modification_time_valid());
    test_date_time(it->get_modification_time(), 19h, 56min, 7s, 8d, std::chrono::January, 2069y);

    print_stack_if_false(!it->is_size_valid());
}

void ParserTest::check_file3(Parser::Objects::const_iterator it) {
    std::print("Testing file 3…\n");
    print_stack_if_false(it->get_name() == "Тестовый файл 3.txt");
    print_stack_if_false(it->get_type() == FileSystemObject::Type::File);

    print_stack_if_false(it->is_creation_time_valid());
    test_date_time(it->get_creation_time(), 11h, 15min, 43s, 18d, std::chrono::March, 2025y);

    print_stack_if_false(it->is_modification_time_valid());
    test_date_time(it->get_modification_time(), 19h, 56min, 7s, 8d, std::chrono::January, 1970y);

    print_stack_if_false(!it->is_size_valid());
}
