/**
 * std::condition_variable
 * cv.wait(lk, predicate)
 * cv.wait_for(lk, duration, predicate)
 * cv.wait_until(lk, time_point, predicate)
 * cv.notify_one()
 * cv.notify_all()
 */

#include <condition_variable>
#include <mutex>

int main() {
  std::mutex mtx;
  std::unique_lock<std::mutex> lk(mtx);
  std::condition_variable cv;
  cv.wait(lk, []() { return true; });
  return 0;
}