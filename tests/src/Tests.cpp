#include "Tests.h"

#include "SizeDisplayerTest.h"

namespace {
    void message_handler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {}
}

Tests::Tests() { qInstallMessageHandler(&message_handler); }

void Tests::run() {
    _parser_test.run();
    SizeDisplayerTest::run();
}
