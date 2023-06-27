#include <iostream>
#include "spdlog/spdlog.h"

int main(int, char**) {
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::info("Support for floats and bools {0:.2f}, {1:}", 1.23456, true);
    return 0;
}
