#ifndef X4_THREAD_H_
#define X4_THREAD_H_

#include <atomic>
#include <cassert>
#include <thread>

#ifdef NDEBUG
#define X4_THREAD_LOG_DEBUG(s, i) ((void)0)
#else
#include <iostream>
#define X4_THREAD_LOG_DEBUG(s, i) std::cout << s << " " << i << '\n'
#endif

namespace x4 {
class mutex {
 private:
  std::atomic_int c;

 public:
  constexpr mutex() noexcept {}

  mutex(const mutex&) = delete;
  mutex(mutex&&) = delete;
  mutex& operator=(const mutex& other) = delete;
  mutex& operator=(mutex&& other) = delete;

  bool try_lock() {
    if (c != 0) {
      X4_THREAD_LOG_DEBUG("Lock is busy", "");
      return false;
    }
    int c_prev = c.fetch_add(1);
    assert(c_prev >= 0);
    if (c_prev == 0) {
      X4_THREAD_LOG_DEBUG("Locked!", "");
      return true;
    }
    int c_prev_sub = c.fetch_sub(1);
    assert(c_prev_sub >= 0);
    X4_THREAD_LOG_DEBUG("Failed to lock", "");
    return false;
  }

  void lock() {
    while (!try_lock()) {
      std::this_thread::yield();
    }
  }

  void unlock() {
    int c_prev = c.fetch_sub(1);
    assert(c_prev >= 1);
    X4_THREAD_LOG_DEBUG("Unlocked", "");
  }
};

}  // namespace x4

#endif  // X4_THREAD_H_
