#ifndef X4_THREAD_H_
#define X4_THREAD_H_

#include <err.h>
#include <linux/futex.h>
#include <sys/errno.h>
#include <sys/syscall.h>

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <type_traits>

#ifdef NDEBUG
#define X4_THREAD_LOG_DEBUG(s, i) ((void)0)
#else
#include <iostream>
#define X4_THREAD_LOG_DEBUG(s, i) std::cout << s << " " << i << '\n'
#endif

namespace x4 {
class mutex_futex {
  std::atomic_uint32_t futex1{0};  // unlocked

 public:
  constexpr mutex_futex() noexcept {}

  bool try_lock() {
    if (futex1.exchange(1) == 0) {
      X4_THREAD_LOG_DEBUG("Locked!", "");
      return true;
    }
    X4_THREAD_LOG_DEBUG("Lock is busy", "");
    return false;
  }

  void lock() {
    long s;
    while (!try_lock()) {
      s = syscall(SYS_futex, (uint32_t*)&futex1, FUTEX_WAIT, 1, NULL, NULL, 0);
      if (s == -1 && errno != EAGAIN) err(EXIT_FAILURE, "futex-FUTEX_WAIT");
    }
  }

  void unlock() {
    long s;
    if (futex1.exchange(0) != 1) {
      err(EXIT_FAILURE, "futex was not locked");
    }
    // val = 1, so wake up a single waiter
    s = syscall(SYS_futex, (uint32_t*)&futex1, FUTEX_WAKE, 1, NULL, NULL, 0);
    if (s == -1) err(EXIT_FAILURE, "futex-FUTEX_WAKE");
    X4_THREAD_LOG_DEBUG("Unlocked", "");
  }
};

class mutex {
 private:
  std::atomic_bool busy_flag;
  // std::atomic_int next;
  // std::atomic_int ticket;

 public:
  mutex() noexcept {}

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
      //_mm_pause();
      //__asm volatile ("pause" ::: "memory");
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

using Task = std::function<void()>;

template <typename T>
class BlockingQueue {
 public:
  void push(T&& v) {
    std::lock_guard<std::mutex> lk{m};
    queue_.push(std::move(v));
    cv.notify_one();
  }

  void push(const T&) = delete;

  T pop() {
    std::unique_lock<std::mutex> lk{m};
    while (queue_.empty() && !stop_flag) {
      X4_THREAD_LOG_DEBUG("Waiting", "");
      cv.wait(lk);
      X4_THREAD_LOG_DEBUG("Notified", "");
    }
    if (stop_flag) {
      return T();
    }
    X4_THREAD_LOG_DEBUG("Reading front", "");
    T task = std::move(queue_.front());
    queue_.pop();
    return task;
  }
  void stop() {
    stop_flag.store(true);
    cv.notify_all();
  }
  size_t size() const { return queue_.size(); }

  bool is_stopped() const { return stop_flag; }

 private:
  std::queue<T> queue_;
  std::mutex m;
  std::condition_variable cv;
  std::atomic_bool stop_flag;
};

class ThreadPool {
 public:
  ThreadPool(size_t capacity) : capacity(capacity) {
    threads.reserve(capacity);
    for (int i = 0; i < capacity; i++) {
      threads.emplace_back([this] { this->runner(); });
    }
  }

  ~ThreadPool() {
    X4_THREAD_LOG_DEBUG("[Pool Destructor]", "Stop/Join");
    stop();
    join();
  }

  void submit(Task&& task) { task_q.push(std::move(task)); }

  void join() {
    for (std::thread& th : threads) {
      if (th.joinable()) th.join();
    }
  }

  void stop() { task_q.stop(); }

  size_t get_queue_size() const { return task_q.size(); }

  bool is_stopped() const { return task_q.is_stopped(); }

 private:
  size_t capacity;
  BlockingQueue<Task> task_q;
  std::vector<std::thread> threads;
  std::atomic_bool stop_flag;

  void runner() {
    while (true) {
      if (is_stopped()) return;
      auto task = task_q.pop();
      if (is_stopped()) return;
      task();
    }
  }
};

}  // namespace x4

#endif  // X4_THREAD_H_
