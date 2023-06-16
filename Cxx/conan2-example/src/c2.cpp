#include <boost/regex.hpp>
#include <iostream>
#include <string>

using namespace std;

bool validate_card_format(const std::string &s) {
  static const boost::regex e("(\\d{4}[- ]){3}\\d{4}");
  return boost::regex_match(s, e);
}

int main(int, char **) {
  int a = 8;
  std::size_t buf_size = 80;
  char buf[buf_size];
  snprintf(buf, buf_size, "num: %d", a);
  std::cout << "Hello, from c2!, " << buf << std::endl;
  auto cc_num = "0000-1111-2222-3333";
  auto is_cc_valid = validate_card_format(cc_num);
  cout << "validate_card_format(\"" << cc_num << "\") returned "
       << (is_cc_valid ? "True" : "False") << endl;
  return 0;
}
