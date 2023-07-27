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
  x4::ThreadPool thpool{6};
  fmt::println("[main] pool created. sleep 1s");
  std::this_thread::sleep_for(1s);
  fmt::println("[main] woke up");
  fmt::println("[main] Submit");
  auto t0 = high_resolution_clock::now();
  int N = 32000;
  for (int i = 0; i < N; i++) {
    thpool.submit(f2);
    // thpool.submit([]() { fmt::println("F2 test"); });
  }
  fmt::println("[main] spin lock till empty queue");
  std::this_thread::yield();
  while (thpool.get_queue_size() != 0) {
    std::this_thread::yield();
  }
  while (true) {
    {
        std::lock_guard<std::mutex> lk(m);
        if (i == N) {
            break;
        }
    }
    std::this_thread::yield();
  }
  std::chrono::duration<double, std::milli> dur_in_ms{high_resolution_clock::now() - t0};
  fmt::println("pool queue size: {}, i: {}, duration (ms): {}", thpool.get_queue_size(), i, dur_in_ms.count());
  fmt::println("[main] woke up");
  fmt::println("[main] Stop/Join");

  thpool.stop();
  thpool.join();

  fmt::println("i: {}", i);

  return 0;
}
