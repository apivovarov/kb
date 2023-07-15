#include <fmt/core.h>

#include <chrono>
#include <iostream>
#include <vector>

#include "x4/thread.hpp"

static x4::mutex m;

static int a = 0;

void f() {
  //   while (!m.try_lock()) {
  //     std::this_thread::yield();
  //   }
  m.lock();
  a += 1;
  m.unlock();
}

void test_atomic() {
  std::atomic_int c(0);
  int c_prev = c.fetch_add(1);
  std::cout << "c: " << c << " " << c.load() << " " << c_prev << "\n";
  if (c == 1) {
    std::cout << "one\n";
  }
  c_prev = c.fetch_sub(1);
  std::cout << "c: " << c << " " << c.load() << " " << c_prev << "\n";
  if (c == 0) {
    std::cout << "zero\n";
  }
}

int main() {
  auto t0 = std::chrono::high_resolution_clock::now();
  int N = 32000;
  std::vector<std::thread> ths;
  for (int i = 0; i < N; i++) {
    ths.emplace_back(f);
    std::this_thread::yield();
  }
  for (auto& th : ths) {
    th.join();
    std::this_thread::yield();
  }

  fmt::println("a: {:}", a);

  if (a != N) {
    throw std::invalid_argument("a != N");
  }
  std::chrono::duration<double> sec(std::chrono::high_resolution_clock::now() -
                                    t0);
  fmt::println("Duration: {} sec", sec.count());
  return 0;
}
