#include <fmt/format.h>

#include <memory>

#include "x4/memory.hpp"

void print(int aa) {
  int a = aa + 2;
  int b = 1 + a;
  fmt::println("aa: {}, a: {}, b: {}", fmt::ptr(&aa), fmt::ptr(&a), fmt::ptr(&b));
}

struct AA {
  int i;

  AA() { i = 10; };

  AA(const int i) { this->i = i; }

  int getI() const noexcept { return i; }
};

int main() {
  int cc = 1;
  int dd = 2;
  fmt::println("cc: {}, dd: {}", fmt::ptr(&cc), fmt::ptr(&dd));
  print(cc);

  x4::shared_ptr<AA> p2aa = x4::make_shared<AA>(45);
  fmt::println("p2aa getI(): {}", p2aa->getI());

  x4::shared_ptr<int> i2p(new int(4));
  int* i5 = new int(5);
  //  int* i6 = new int(6);
  int* i7 = nullptr;
  // std::shared_ptr<int> pp (i5);
  std::shared_ptr<int> pp;
  std::shared_ptr<int> pp2 = pp;
  pp.reset(i5);
  pp.reset(i7);
  {
    fmt::println("i2p={}", *(i2p.get()));
    fmt::println("i2p.use_cnt={}", i2p.use_count());
    x4::shared_ptr<int> i3p(i2p);
    fmt::println("i2p={}", *(i2p.get()));
    fmt::println("i2p.use_cnt={}", i2p.use_count());
    fmt::println("i3p={}", *i3p);
    fmt::println("i3p.use_cnt={}", i3p.use_count());
    {
      x4::shared_ptr<int> i4p(std::move(i2p));
      // i3p = i2p;

      fmt::println("i2p.ptr={}", fmt::ptr(i2p.get()));
      fmt::println("i2p.use_cnt={}", i2p.use_count());
      fmt::println("i3p={}", *i3p);
      fmt::println("i3p.use_cnt={}", i3p.use_count());
      fmt::println("i4p={}", *(i4p.get()));
      fmt::println("i4p.use_cnt={}", i4p.use_count());
      {
        x4::shared_ptr<int> i5p;
        i5p = i2p;
        x4::shared_ptr<int> i6p;
        i6p = std::move(i2p);
        fmt::println("i5p.ptr={}", fmt::ptr(i5p.get()));
        fmt::println("i5p.use_cnt={}", i5p.use_count());
        fmt::println("i6p.ptr={}", fmt::ptr(i6p.get()));
        fmt::println("i6p.use_cnt={}", i6p.use_count());
      }
    }
    // std::shared_ptr<int> pp2 = pp;
    // fmt::println("i={}", *(pp2.get()));
    // fmt::println("pp.cnt={}", pp.use_count());
    // fmt::println("pp2.cnt={}", pp2.use_count());
    // fmt::println("pp.ptr={}", fmt::ptr(pp.get()));
    // fmt::println("pp.ref={:p}", i5);

    fmt::println("i2p.ptr={}", fmt::ptr(i2p.get()));
    fmt::println("i2p.use_cnt={}", i2p.use_count());
    fmt::println("i3p={}", *(i3p.get()));
    fmt::println("i3p.use_cnt={}", i3p.use_count());
  }
  // fmt::println("i2p.use_cnt={}", i2p.use_count());

  // pp.reset(i6);
  //   int zz=6;
  //   fmt::println("i6={:p}", fmt::ptr(new int(6)));
  //   fmt::println("zz={:p}", fmt::ptr(&zz));

  fmt::println("i2p.ptr={}", fmt::ptr(i2p.get()));
  fmt::println("i2p.use_cnt={}", i2p.use_count());

  int* i2 = new int(10);
  fmt::println("i2={}", (*i2)--);
  fmt::println("i2={}", --(*i2));
  i2p.reset(i2);
  fmt::println("i2p={}", *(i2p.get()));
  fmt::println("i2p.use_cnt={}", i2p.use_count());

  return 0;
}
