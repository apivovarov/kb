#include "x4/thread.hpp"
#include <fmt/core.h>

void f1() {
    fmt::println("F1 test");
}

int main() {

    x4::ThreadPool::Task t1 = f1;
    x4::ThreadPool thpool{};
    thpool.submit(f1);
    thpool.submit([]() {
        fmt::println("F2 test");
    });
    thpool.run();
    thpool.run();


    return 0;
}
