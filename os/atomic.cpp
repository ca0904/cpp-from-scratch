/**
 * std::atomic<T> doesn't allow multiple threads to modify T simultaneously
 */

#include <atomic>
#include <thread>

std::atomic<int> counter;
void func() {
  for (int i = 0; i < 100000; ++i)
    counter++;
}

int main() {
  std::thread t1(func);
  std::thread t2(func);
  t1.join();
  t2.join();
  return 0;
}