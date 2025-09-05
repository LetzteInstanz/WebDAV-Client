#pragma once

#include "../FileSystem/Parser/Parser.h"

class ParserTest
{
public:
    void run();

private:
    void test_correct_response();
    void test_resourcetype_has_non_ok_status();
    void test_resourcetype_is_absent();
    void test_current_dir_without_collection();
    void test_empty_href();
    void test_href_is_absent();
    void test_status_is_absent();
    void test_empty_status();
    void test_unknown_tag();
    void test_incorrect_tag_order();

private:
    Parser _parser;
};
