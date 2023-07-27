#include "x4/thread.hpp"

namespace x4 {

template <typename T>
void BlockingQueue<T>::push(T&& v)
  requires std::is_rvalue_reference<T&&>::value
{
  std::lock_guard<std::mutex> lk{m};
  queue_.push(std::move(v));
  cv.notify_one();
}

template <typename T>
T BlockingQueue<T>::pop() {
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

template <typename T>
void BlockingQueue<T>::stop() {
  stop_flag.store(true);
  cv.notify_all();
}

template <typename T>
size_t BlockingQueue<T>::size() const { return queue_.size(); }

template <typename T>
bool BlockingQueue<T>::is_stopped() const { return stop_flag; }

}  // namespace x4
