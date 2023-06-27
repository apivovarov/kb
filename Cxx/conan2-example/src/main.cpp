#include <boost/regex.hpp>
#include <iostream>
#include "spdlog/spdlog.h"

bool validate_card_format(const std::string &s) {
  static const boost::regex e("(\\d{4}[- ]){3}\\d{4}");
  return boost::regex_match(s, e);
}

int main(int, char**) {
    spdlog::info("Welcome to spdlog!");
    spdlog::error("Some error message with arg: {}", 1);
    spdlog::warn("Easy padding in numbers like {:08d}", 12);
    spdlog::info("Support for floats {0:.2f} and bools {1:}", 1.23456, true);
    auto cc_num = "0000-1111-2222-3333";
    auto is_cc_valid = validate_card_format(cc_num);
    spdlog::info("validate_card_format(\"{}\") returned {}", cc_num, is_cc_valid);
    return 0;
}
