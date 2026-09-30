/**
 * Mutex: Mutual Exclusion
 *
 * This simple implementation uses busy-waiting (spinning) and the
 * compare-and-swap (CAS) atomic primitive to ensure that only one thread can
 * acquire the mutex at a time.
 *
 * compare_exchange_strong(expected, desired) atomically compares locked with
 * expected. If they are equal, it sets locked to desired and returns true.
 * Otherwise, it updates expected with the current value of locked and returns
 * false. We also use std::memory_order_acquire on lock and
 * std::memory_order_release on unlock to ensure proper ordering of memory
 * operations across threads.
 */

/**
 * std::mutex
 * lock() will block the thread if lock() is not successful
 * try_lock() will not block return bool value
 * unlock() will unlock the lock
 * If lock() is called twice by same thread then deadlock
 */

/**
 * std::lock_guard<> lk(mtx);
 * It automatically locks the mutex when created
 * It automatically unlocks the mutex when destroyed
 * std::lock_guard is neither copyable nor movable
 */

/**
 * std::unique_lock<> lk(mtx);
 * It automatically locks the mutex when created
 * It automatically unlocks the mutex when destroyed
 * std::unique_lock is movable but not copyable
 * lk.unlock() can be called manually
 * lk(mtx, std::defer_lock) can defer locking
 * lk(mtx, std::try_to_lock) can try locking
 * lk(mtx, std::adopt_lock) can adopt already locked mutex
 * Can use with std::condition_variable
 */

/**
 * std::shared_mutex
 * std::shared_lock
 * Multiple threads can hold shared_lock simultaneously
 * Only one thread can hold unique lock at a time
 */

/**
 * std::timed_mutex
 * lock(), try_lock(), unlock() same as std::mutex
 * try_lock_for() will waits up to given duration
 * try_lock_until() waits until specific time point
 */

/**
 * std::recursive_mutex
 * Can call lock() multiple times
 * Prevents self-deadlock
 * Need to call unlock() exact number of times as lock()
 * std::recursive_timed_mutex is combination of both
 */

/**
 * std::scoped_lock lk(mtx1, mtx2, ...);
 * Locks multiple mutexes avoiding deadlocks
 * Automatically unlocks all mutexes when destroyed
 */

#include <atomic>
#include <mutex>
#include <stdexcept>
#include <thread>

class Mutex {
private:
  std::atomic<int> locked;

public:
  Mutex() { locked.store(0); }

  void lock() {
    int expected = 0;
    using std::memory_order_acquire;
    while (!locked.compare_exchange_strong(expected, 1, memory_order_acquire))
      expected = 0;
  }

  void unlock() {
    int expected = 1;
    using std::memory_order_release;
    if (!locked.compare_exchange_strong(expected, 0, memory_order_release))
      throw std::logic_error("unlock called without locking");
  }
};

int main() {
  std::mutex mtx;
  mtx.lock();
  mtx.unlock();
  if (mtx.try_lock())
    mtx.unlock();

  // The spinlock above guarding a shared counter
  Mutex m;
  int counter = 0;
  auto work = [&]() {
    for (int i = 0; i < 100000; ++i) {
      m.lock();
      ++counter;
      m.unlock();
    }
  };
  std::thread t1(work), t2(work);
  t1.join();
  t2.join();
  return counter == 200000 ? 0 : 1;
}