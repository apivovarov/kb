#include <fmt/core.h>
#include <fmt/ostream.h>

#include <forward_list>
#include <iostream>
#include <memory>

struct BBB {
  BBB() { std::cout << "BBB(default)\n"; }
  BBB(const BBB& o) { std::cout << "BBB(ref)\n"; }
  BBB(BBB&& o) { std::cout << "BBB(rval ref)\n"; }
  BBB& operator=(const BBB& other) {
    std::cout << "BBB::=(ref)\n";
    return *this;
  }
  BBB& operator=(BBB&& other) noexcept {
    std::cout << "BBB::=(rval ref)\n";
    return *this;
  }
  friend std::ostream& operator<<(std::ostream& os, BBB const& x) {
    os << "BBB(" << ')';
    return os;
  }
};

template <>
struct fmt::formatter<BBB> : ostream_formatter {};

struct AAA {
  int a, b, c;
  friend std::ostream& operator<<(std::ostream& os, AAA const& x) {
    os << "AAA(" << x.a << ',' << x.b << ',' << x.c << ')';
    return os;
  }
};

template <>
struct fmt::formatter<AAA> : ostream_formatter {};

int main() {
  using AAA_ptr = std::unique_ptr<AAA>;
  std::forward_list<AAA_ptr> list;
  auto p1 = std::make_unique<AAA>(1,2,3);
  p1->a = 10;
  list.push_front(std::move(p1));
  list.emplace_front(std::make_unique<AAA>(4,5,6));

  for (AAA_ptr& v : list) {
    // fmt::println("{}", AAA(1, 2, 3));
    //  auto s = AAA(1,2,3);
    // std::cout << *v << "\n";
    fmt::println("{}", *v);
  }

  return 0;
}
