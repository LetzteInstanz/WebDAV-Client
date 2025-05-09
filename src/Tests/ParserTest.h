#pragma once

#include "../FileSystem/Parser/Parser.h"

class ParserTest
{
public:
    static void run();

private:
    static void check_current_dir(const Parser::CurrDirObj& current_dir);
    static void check_dir1(Parser::Objects::const_iterator it);
    static void check_dir2(Parser::Objects::const_iterator it);
    static void check_file1(Parser::Objects::const_iterator it);
    static void check_file2(Parser::Objects::const_iterator it);
    static void check_file3(Parser::Objects::const_iterator it);
};
