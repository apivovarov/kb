#include <fmt/core.h>

#include <chrono>
#include <iostream>
#include <vector>
//#include <thread>

#include "x4/thread.hpp"

static x4::mutex_futex m;
//static std::mutex m_std;
static const int N = 32000;
static double times[N];

static int a = 0;

void f(int i) {
  //   while (!m.try_lock()) {
  //     std::this_thread::yield();
  //   }
  auto t0 = std::chrono::high_resolution_clock::now();
  {
    x4::lock_guard<x4::mutex_futex> guard(m);
    a += 1;
  }
  std::chrono::duration<double> sec(std::chrono::high_resolution_clock::now() -
                                    t0);
  times[i] = sec.count();
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

  std::vector<std::thread> ths;
  for (int i = 0; i < N; i++) {
    ths.emplace_back(f, i);
    //std::this_thread::yield();
  }
  for (auto& th : ths) {
    th.join();
    //std::this_thread::yield();
  }

  fmt::println("a: {:}", a);

  if (a != N) {
    throw std::invalid_argument("a != N");
  }
  std::chrono::duration<double> sec(std::chrono::high_resolution_clock::now() -
                                    t0);
  fmt::println("Duration: {} sec", sec.count());

  auto max = *std::max_element(times, times + N);
  auto min = *std::min_element(times, times + N);

  fmt::println("Min f() time: {}", min);
  fmt::println("Max f() time: {}", max);

  return 0;
}
