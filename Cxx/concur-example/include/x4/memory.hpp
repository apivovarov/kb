#ifndef X4_MEMORY_H_
#define X4_MEMORY_H_

#ifdef NDEBUG
#define X4_MEM_LOG_DEBUG(s) ((void)0)
#else
#include <iostream>
#define X4_MEM_LOG_DEBUG(s) std::cout << s << '\n'
#endif

namespace x4 {

template <class T>
class shared_ptr {
 private:
  T* ptr;
  int* cnt;

 public:
  shared_ptr() {
    ptr = nullptr;
    cnt = nullptr;
    X4_MEM_LOG_DEBUG("default Ctor");
  }

  shared_ptr(T* ptr) {
    this->ptr = ptr;
    cnt = new int(1);
    X4_MEM_LOG_DEBUG("Ctor with pointer");
  }

  ~shared_ptr() {
    if (cnt) {
      if (*cnt == 1) {
        if (ptr) {
          delete ptr;
        }
        if (cnt) {
          delete cnt;
        }
        X4_MEM_LOG_DEBUG("Dtor deletes ptr");
      } else {
        *cnt -= 1;
        X4_MEM_LOG_DEBUG("Dtor cnt minus");
      }
    } else {
      X4_MEM_LOG_DEBUG("Dtor no pointer");
    }
  }

  shared_ptr(const shared_ptr& other) {
    X4_MEM_LOG_DEBUG("copy Ctor");
    cnt = other.cnt;
    ptr = other.ptr;
    if (cnt) {
      *cnt += 1;
    }
  }

  shared_ptr(shared_ptr&& other) {
    X4_MEM_LOG_DEBUG("move Ctor");
    cnt = other.cnt;
    ptr = other.ptr;
    other.cnt = nullptr;
    other.ptr = nullptr;
  }
  // LLVM, GNU, Google, Chromium, Microsoft, Mozilla, WebKit.

  shared_ptr& operator=(const shared_ptr& other) {
    X4_MEM_LOG_DEBUG("copy assign");
    cnt = other.cnt;
    ptr = other.ptr;
    if (cnt) {
      ++(*cnt);
    }
    return *this;
  }

  shared_ptr& operator=(shared_ptr&& other) {
    X4_MEM_LOG_DEBUG("move assign");
    cnt = other.cnt;
    ptr = other.ptr;
    other.cnt = nullptr;
    other.ptr = nullptr;
    return *this;
  }

  void reset(T* ptr) {
    if (cnt) {
      if (*cnt == 1) {
        if (ptr) {
          delete ptr;
        }
        if (cnt) {
          delete cnt;
        }
        X4_MEM_LOG_DEBUG("reset deletes ptr");
      } else {
        --(*cnt);
        X4_MEM_LOG_DEBUG("reset cnt minus");
      }
    }
    this->ptr = ptr;
    cnt = new int(1);
  }

  T* get() const noexcept { return this->ptr; }

  int use_count() const noexcept {
    if (cnt) {
      return *cnt;
    }
    return 0;
  }

  T& operator*() const noexcept { return *ptr; }

  T* operator->() const noexcept { return ptr; }
};

template <class T, class... Args>
shared_ptr<T> make_shared(Args&&... args) {
  return shared_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace x4

#endif  // X4_MEMORY_H_
