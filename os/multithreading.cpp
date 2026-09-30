#include <thread>

void func(int) {}

class A {
public:
  void operator()(int) {}
  int run(int x) { return x; }
  static int run2(int x) { return x; }
};

int main() {
  A a;
  // Different Ways to create threads
  std::thread t1(func, 1);
  std::thread t2([](int) -> void {}, 1);
  std::thread t3(A(), 5); // lvalue/rvalue A
  std::thread t4(&A::run, &a, 11);
  std::thread t5(&A::run2, 8);
  t1.join();
  t2.join();
  t3.join();
  t4.join();
  t5.join();
  /**
   * If join() called twice then it will terminate the main() program
   * Can use joinable() function to check, if it already joined or not
   * detach() doesn't wait for thread excuetion to complete
   * detach() also can't be called twice, can use joinable() to check
   * Use join/detach else program will throw error due to thread destructor
   * detach() can be used for background tasks (logging, cleanup, etc.)
   */
  return 0;
}