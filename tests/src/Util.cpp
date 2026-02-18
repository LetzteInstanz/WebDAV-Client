#include "Util.h"

void print_stack_if_false(bool expr) {
    if (!expr)
        std::cout << "\033[1;31mFailed:\033[0m" << std::endl << std::stacktrace::current() << std::endl;
}
