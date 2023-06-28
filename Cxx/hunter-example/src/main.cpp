#include <iostream>

#include "spdlog/spdlog.h"
#include <fmt/format.h>

#include <boost/regex.hpp>

void test_fmt() {
  fmt::print("Hello world\nThis is fmt(ex-cppformat)\n");
  std::string as_string = fmt::format("The answer is {}", 42);
  auto buf = fmt::memory_buffer();
  fmt::format_to(
      std::back_inserter(buf),
      "{}\nThe previous line and this message were bufferred in memory",
      as_string);
  fmt::print(stderr, "{}\nAnd then were printed to stderr\n",
             fmt::to_string(buf));
  fmt::print("Fmt supports many nice features, see {url} for details\n",
             fmt::arg("url", "https://github.com/fmtlib/fmt"));
}

void test_spdlog() {
  spdlog::info("Welcome to spdlog!");
  spdlog::error("Some error message with arg: {}", 1);
  spdlog::warn("Easy padding in numbers like {:08d}", 12);
  spdlog::info("Support for floats {0:.2f} and bools {1:}", 1.23456, true);
}

bool validate_card_format(const std::string &s) {
  static const boost::regex e("(\\d{4}[- ]){3}\\d{4}");
  return boost::regex_match(s, e);
}

int main(int, char **) {
  test_fmt();
  test_spdlog();
  auto cc_num = "0000-1111-2222-3333";
  auto is_cc_valid = validate_card_format(cc_num);
  spdlog::info("validate_card_format(\"{}\") returned {}", cc_num, is_cc_valid);
  return 0;
}
