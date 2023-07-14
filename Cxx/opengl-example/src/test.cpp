#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>

using std::endl, std::cout, std::cerr;

std::condition_variable cv;
std::mutex cv_m; // This mutex is used for three purposes:
                 // 1) to synchronize accesses to i
                 // 2) to synchronize accesses to std::cerr
                 // 3) for the condition variable cv
//int i = 0;

void waits() {
  for (int i = 0; i < 19; i++) {
    std::unique_lock<std::mutex> lk(cv_m);
    auto tid = std::this_thread::get_id();
    std::cerr << tid << ": Waiting..." << endl;
    cv.wait(lk); //, [] { return i == 1; });
    std::cerr << tid << ": Notified" << endl;
  }
}

void signals() {
  for (int i = 0; i < 20; i++) {
    std::this_thread::sleep_for(std::chrono::seconds(4));
    {
      std::lock_guard<std::mutex> lk(cv_m);
      std::cerr << "Notifying..." << endl;
    }
    cv.notify_one();
  }
}

int main() {
  std::thread t1(waits), t2(waits), t3(waits);
  std::thread t4(signals);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
}
