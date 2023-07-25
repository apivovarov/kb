#include <fmt/format.h>

#include <chrono>

#include "x4/thread.hpp"

void f1() { fmt::println("F1 test"); }

static std::mutex m;
static int i = 0;

static void f2() {
  std::lock_guard<std::mutex> lk(m);
  i += 1;
}

int main() {
  using namespace std::chrono;
  x4::ThreadPool thpool{12};
  fmt::println("[main] pool created. sleep 1s");
  std::this_thread::sleep_for(1s);
  fmt::println("[main] woke up");
  fmt::println("[main] Submit");
  for (int i = 0; i < 32000; i++) {
    thpool.submit(f2);
    // thpool.submit([]() { fmt::println("F2 test"); });
  }
  fmt::println("[main] sleep 1ms");
  std::this_thread::sleep_for(1ms);
  fmt::println("pool queue size: {}", thpool.get_queue_size());
  fmt::println("[main] woke up");
  fmt::println("[main] Stop/Join");

  thpool.stop();
  thpool.join();

  fmt::println("i: {}", i);

  return 0;
}
