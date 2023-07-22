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
  std::atomic_bool busy_flag;
  // std::atomic_int next;
  // std::atomic_int ticket;

 public:
  constexpr mutex() noexcept {}

  mutex(const mutex&) = delete;
  mutex(mutex&&) = delete;
  mutex& operator=(const mutex& other) = delete;
  mutex& operator=(mutex&& other) = delete;

  bool try_lock() {
    if (busy_flag) {
      X4_THREAD_LOG_DEBUG("Lock is busy", "");
      return false;
    }
    bool busy_flag_prev = busy_flag.exchange(true);
    if (!busy_flag_prev) {
      X4_THREAD_LOG_DEBUG("Locked!", "");
      return true;
    }
    X4_THREAD_LOG_DEBUG("Failed to lock", "");
    return false;
  }

  void lock() {
    while (!try_lock()) {
      busy_flag.wait(true);
      // std::this_thread::yield();
    }
  }

  void unlock() {
    busy_flag.store(false);
    busy_flag.notify_one();
    X4_THREAD_LOG_DEBUG("Unlocked", "");
  }

  // void lock2() {
  //   int ticket_ = ticket.fetch_add(1);
  //   while (true) {
  //     int next_ = next.load();
  //     if (next_ == ticket_) {
  //       break;
  //     }
  //     X4_THREAD_LOG_DEBUG("Waiting!", "");
  //     next.wait(next_);
  //   }
  //   X4_THREAD_LOG_DEBUG("Locked!", "");
  // }

  // void unlock2() {
  //   next.fetch_add(1);
  //   next.notify_all();
  //   X4_THREAD_LOG_DEBUG("Unlocked", "");
  // }
};

template <class T>
class lock_guard {
 private:
  T& m;
  lock_guard(lock_guard const&) = delete;
  lock_guard& operator=(lock_guard const&) = delete;

 public:
  lock_guard(T& m_) : m(m_) { m.lock(); }
  ~lock_guard() { m.unlock(); }
};

}  // namespace x4

#endif  // X4_THREAD_H_
