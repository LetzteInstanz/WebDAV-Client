#include "SizeDisplayerTest.h"

#include "../Qml/FileItemModel/SizeDisplayer.h"
#include "Util.h"

void SizeDisplayerTest::run() {
    std::print("Testing size displayer…\n");
    test_rounding();
}

void SizeDisplayerTest::test_rounding() {
    std::print("Testing rounding…\n");
    QString str = SizeDisplayer::to_string(1023);
    print_stack_if_false(str.indexOf(QObject::tr("B")) != -1);

    str = SizeDisplayer::to_string(1024);
    print_stack_if_false(str.indexOf(QObject::tr("K")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 2) - 1);
    print_stack_if_false(str.indexOf(QObject::tr("K")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 2));
    print_stack_if_false(str.indexOf(QObject::tr("M")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 3) - 1);
    print_stack_if_false(str.indexOf(QObject::tr("M")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 3));
    print_stack_if_false(str.indexOf(QObject::tr("G")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 4) - 1);
    print_stack_if_false(str.indexOf(QObject::tr("G")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 4));
    print_stack_if_false(str.indexOf(QObject::tr("T")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 5) - 1);
    print_stack_if_false(str.indexOf(QObject::tr("T")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 5));
    print_stack_if_false(str.indexOf(QObject::tr("P")) != -1);

    //str = SizeDisplayer::to_string(std::pow(1024, 6) - 1); // note: the precision of double type isn't enough; long double isn't supported by Qt
    //print_stack_if_false(str.indexOf(QObject::tr("P")) != -1);

    str = SizeDisplayer::to_string(std::pow(1024, 6));
    print_stack_if_false(str.indexOf(QObject::tr("E")) != -1);

    str = SizeDisplayer::to_string(0xFFFFFFFFFFFFFFFF);
    print_stack_if_false(str.indexOf(QObject::tr("E")) != -1);
}
