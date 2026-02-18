#pragma once

void print_stack_if_false(bool expr);

template<typename ExceptionType>
void print_stack_if_no_exception(const std::function<void ()>& func) {
    try {
        func();
        print_stack_if_false(false);
    } catch (const ExceptionType&) {}
}
