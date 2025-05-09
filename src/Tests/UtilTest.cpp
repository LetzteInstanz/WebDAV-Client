#include "UtilTest.h"

#include "../Util.h"
#include "Util.h"

void UtilTest::run() {
    std::print("Testing util functions…\n");
    test_processing_of_two_dots_in_path();
}

void UtilTest::test_processing_of_two_dots_in_path() {
    std::print("Testing process_two_dots_in_path()…\n");
    QString test = "/";
    print_stack_if_false(process_two_dots_in_path(test) == "/");
    test = "/test/test2/../../test3/test4/";
    print_stack_if_false(process_two_dots_in_path(test) == "/test3/test4/");
    test = "/test/../test2/../test3/";
    print_stack_if_false(process_two_dots_in_path(test) == "/test3/");
    test = "/test/..g/";
    print_stack_if_false(process_two_dots_in_path(test) == test);
    test = "/test/g../";
    print_stack_if_false(process_two_dots_in_path(test) == test);
    test = "/test/..../";
    print_stack_if_false(process_two_dots_in_path(test) == test);
}
