/**
 * std::promise
 * set_value() to set the value
 * set_exception() to set an exception
 * get_future() to get the associated future
 * All methods are can be called only once
 * promise are not copyable always use std::move for thread/function arguments
 *
 * std::future
 * get() to get the value (blocks if not ready)
 * wait() to wait until the value is ready
 * wait_for() and wait_until() for timed waits
 * valid() to check if the future is valid
 * After calling get(), the future becomes invalid
 *
 * std::async
 * std::launch::async - asynchronously
 * std::launch::deferred - only when the result is needed
 */

#include <future>
#include <thread>

int main() {
  std::promise<int> prom;
  std::future<int> fut = prom.get_future();

  // can use &&p as well
  std::thread t([](std::promise<int> p) { p.set_value(42); }, std::move(prom));
  [[maybe_unused]] bool before = fut.valid(); // true
  [[maybe_unused]] int value = fut.get();     // 42
  [[maybe_unused]] bool after = fut.valid();  // false
  t.join();

  std::future<int> fut2 = std::async(std::launch::async, []() { return 7; });

  return 0;
}