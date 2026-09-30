#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <semaphore>

class ProducerConsumerCV {
private:
  std::size_t capacity;
  std::mutex mtx;
  std::queue<int> buffer;
  // Separate conditions: with one shared cv, notify_one can wake a thread of
  // the same kind (consumer -> consumer) and the one that could proceed sleeps
  // forever
  std::condition_variable not_full, not_empty;

public:
  ProducerConsumerCV(std::size_t capacity) : capacity(capacity) {}

  void producer(int item) {
    std::unique_lock lk(mtx);
    not_full.wait(lk, [&]() { return buffer.size() < capacity; });
    buffer.push(item);
    not_empty.notify_one();
  }

  int consumer() {
    std::unique_lock lk(mtx);
    not_empty.wait(lk, [&]() { return !buffer.empty(); });
    int item = buffer.front();
    buffer.pop();
    not_full.notify_one();
    return item;
  }
};

class ProducerConsumer {
private:
  std::mutex mtx;
  std::queue<int> buffer;
  std::counting_semaphore<> empty;
  std::counting_semaphore<> full;

public:
  ProducerConsumer(std::size_t capacity) : empty(capacity), full(0) {}

  void producer(int item) {
    empty.acquire();
    mtx.lock();
    buffer.push(item);
    mtx.unlock();
    full.release();
  }

  int consumer() {
    full.acquire();
    mtx.lock();
    int item = buffer.front();
    buffer.pop();
    mtx.unlock();
    empty.release();
    return item;
  }
};