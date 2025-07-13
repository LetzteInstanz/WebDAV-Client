#include "Tests.h"

#include "ParserTest.h"
#include "SizeDisplayerTest.h"
#include "UtilTest.h"

namespace {
    void message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {}
}

Tests::Tests() { qInstallMessageHandler(&message_handler); }

void Tests::run() const {
    ParserTest::run();
    UtilTest::run();
    SizeDisplayerTest::run();
}
