#include "Tests.h"

#include "ParserTest.h"
#include "SizeDisplayerTest.h"
#include "UtilTest.h"

void Tests::run() const {
    ParserTest::run();
    UtilTest::run();
    SizeDisplayerTest::run();
}
