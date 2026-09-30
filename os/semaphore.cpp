/**
 * mutex vs semaphore
 * mutex is binary (locked/unlocked)
 * semaphore is counting (N resources)
 *
 * std::binary_semaphore - mutex equivalent
 * std::counting_semaphore<N> - semaphore with N resources
 * release() - increment semaphore count
 * acquire() - decrement semaphore count (wait if count is 0)
 */

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <semaphore>
#include <stdexcept>

class Semaphore {
private:
  int cnt, available;
  std::mutex mtx;
  std::condition_variable cv;

public:
  Semaphore(int available = 1) : cnt(available), available(available) {}

  void acquire() {
    std::unique_lock lk(mtx);
    cv.wait(lk, [&]() { return cnt > 0; });
    --cnt;
  }

  void release() {
    std::unique_lock lk(mtx);
    if (cnt == available)
      throw std::logic_error("release called without acquiring lock");
    ++cnt;
    cv.notify_one();
  }
};

class SemaphorePrimitive {
private:
  int available;
  std::atomic<int> cnt;
  std::mutex mtx;

public:
  SemaphorePrimitive(int available = 1) : available(available) {
    cnt.store(available);
  }

  void acquire() {
    while (true) {
      while (cnt == 0) {
      }
      std::lock_guard lk(mtx);
      if (!cnt)
        continue;
      cnt--;
      return;
    }
  }

  void release() {
    std::lock_guard lk(mtx);
    if (cnt == available)
      throw std::logic_error("release called without acquiring lock");
    ++cnt;
  }
};

int main() {
  [[maybe_unused]] std::binary_semaphore bsem(0);
  [[maybe_unused]] std::counting_semaphore<5> csem(5);
  return 0;
}