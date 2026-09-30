#include <condition_variable>
#include <functional>
#include <mutex>
#include <semaphore>

// Naive version: can deadlock when every philosopher holds their left fork and
// waits for the right one. The three classes below avoid that.
class DiningPhilosophers {
private:
  std::mutex forks[5];

public:
  void wants_to_eat(int philosopher, std::function<void()> pick_left_fork,
                    std::function<void()> pick_right_fork,
                    std::function<void()> eat,
                    std::function<void()> put_left_fork,
                    std::function<void()> put_right_fork) {
    int left = philosopher;
    int right = (philosopher + 1) % 5;
    std::lock_guard left_lock(forks[left]);
    std::lock_guard right_lock(forks[right]);
    pick_left_fork();
    pick_right_fork();
    eat();
    put_left_fork();
    put_right_fork();
  }
};

class DiningPhilosophersOrder {
private:
  std::mutex forks[5];

public:
  void wants_to_eat(int philosopher, std::function<void()> pick_left_fork,
                    std::function<void()> pick_right_fork,
                    std::function<void()> eat,
                    std::function<void()> put_left_fork,
                    std::function<void()> put_right_fork) {
    int left = philosopher;
    int right = (philosopher + 1) % 5;
    if (philosopher % 2 == 0) {
      std::lock_guard left_lock(forks[left]);
      std::lock_guard right_lock(forks[right]);
      pick_left_fork();
      pick_right_fork();
      eat();
      put_left_fork();
      put_right_fork();
    } else {
      std::lock_guard right_lock(forks[right]);
      std::lock_guard left_lock(forks[left]);
      pick_right_fork();
      pick_left_fork();
      eat();
      put_right_fork();
      put_left_fork();
    }
  }
};

class DiningPhilosophersSemaphore {
private:
  std::mutex forks[5];
  std::counting_semaphore<4> sem{4};

public:
  void wants_to_eat(int philosopher, std::function<void()> pick_left_fork,
                    std::function<void()> pick_right_fork,
                    std::function<void()> eat,
                    std::function<void()> put_left_fork,
                    std::function<void()> put_right_fork) {
    sem.acquire();
    {
      int left = philosopher;
      int right = (philosopher + 1) % 5;
      std::lock_guard left_lock(forks[left]);
      std::lock_guard right_lock(forks[right]);
      pick_left_fork();
      pick_right_fork();
      eat();
      put_left_fork();
      put_right_fork();
    }
    sem.release();
  }
};

class DiningPhilosophersCV {
private:
  std::mutex mtx;
  std::condition_variable cv;
  bool forks[5]{};

public:
  void wants_to_eat(int philosopher, std::function<void()> pick_left_fork,
                    std::function<void()> pick_right_fork,
                    std::function<void()> eat,
                    std::function<void()> put_left_fork,
                    std::function<void()> put_right_fork) {
    int left = philosopher;
    int right = (philosopher + 1) % 5;
    {
      std::unique_lock lk(mtx);
      cv.wait(lk, [&]() { return !forks[left] && !forks[right]; });
      forks[left] = true;
      forks[right] = true;
    }
    pick_left_fork();
    pick_right_fork();
    eat();
    put_left_fork();
    put_right_fork();
    {
      std::unique_lock lk(mtx);
      forks[left] = false;
      forks[right] = false;
      cv.notify_all();
    }
  }
};