// Concurrency classes, meant to run under ThreadSanitizer: each checks the
// property its class promises, with many threads at once
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

// mutex.cpp and semaphore.cpp have their own demo main()
#define main mutex_demo_main
#include "../os/mutex.cpp"
#undef main
#define main semaphore_demo_main
#include "../os/semaphore.cpp"
#undef main
#include "../os/diningphilosophers.cpp"
#include "../os/producerconsumer.cpp"
#include "../os/readerwriter.cpp"
#include "../os/threadpool.cpp"

template <class F> void inThreads(int n, F f) {
  std::vector<std::thread> threads;
  for (int i = 0; i < n; i++)
    threads.emplace_back(f, i);
  for (auto &t : threads)
    t.join();
}

void updateMax(std::atomic<int> &peak, int now) {
  int p = peak;
  while (now > p && !peak.compare_exchange_weak(p, now)) {
  }
}

void testMutex() {
  Mutex m;
  long long counter = 0;
  inThreads(4, [&](int) {
    for (int i = 0; i < 20000; i++) {
      m.lock();
      ++counter;
      m.unlock();
    }
  });
  assert(counter == 80000);
  bool threw = false;
  try {
    m.unlock();
  } catch (const std::logic_error &) {
    threw = true;
  }
  assert(threw);
  assert(mutex_demo_main() == 0);
  puts("Mutex: OK");
}

// never more than N holders; a bad release throws and leaves the count intact
template <class Sem> void testSemaphore(const char *name) {
  const int N = 3;
  Sem s(N);
  std::atomic<int> inside{0}, peak{0};
  inThreads(8, [&](int) {
    for (int i = 0; i < 300; i++) {
      s.acquire();
      updateMax(peak, ++inside);
      std::this_thread::yield();
      --inside;
      s.release();
    }
  });
  assert(peak >= 1 && peak <= N);
  bool threw = false;
  try {
    s.release();
  } catch (const std::logic_error &) {
    threw = true;
  }
  assert(threw);
  for (int i = 0; i < N; i++)
    s.acquire();
  for (int i = 0; i < N; i++)
    s.release();
  printf("%s: OK\n", name);
}

// every produced item is consumed exactly once, and nothing deadlocks
template <class PC> void testProducerConsumer(const char *name) {
  for (std::size_t capacity : {1, 3, 300, 100000}) {
    PC pc(capacity);
    const int producers = 3, consumers = 3, perThread = 3000;
    std::vector<std::vector<int>> got(consumers);
    std::vector<std::thread> threads;
    for (int p = 0; p < producers; p++)
      threads.emplace_back([&, p] {
        for (int k = 0; k < perThread; k++)
          pc.producer(p * perThread + k);
      });
    for (int c = 0; c < consumers; c++)
      threads.emplace_back([&, c] {
        for (int k = 0; k < perThread; k++)
          got[c].push_back(pc.consumer());
      });
    for (auto &t : threads)
      t.join();
    std::vector<int> all;
    for (auto &g : got)
      all.insert(all.end(), g.begin(), g.end());
    std::sort(all.begin(), all.end());
    for (int i = 0; i < producers * perThread; i++)
      assert(all[i] == i);
  }
  printf("%s: OK\n", name);
}

// readers may overlap, a writer is always alone
template <class RW, class Lock>
void testReaderWriter(const char *name, Lock readLock, Lock readUnlock,
                      Lock writeLock, Lock writeUnlock) {
  RW rw;
  std::atomic<int> readers{0}, writers{0};
  int value = 0;
  inThreads(8, [&](int id) {
    for (int i = 0; i < 400; i++) {
      if (id < 2) {
        (rw.*writeLock)();
        assert(++writers == 1 && readers == 0);
        ++value;
        --writers;
        (rw.*writeUnlock)();
      } else {
        (rw.*readLock)();
        ++readers;
        assert(writers == 0);
        --readers;
        (rw.*readUnlock)();
      }
    }
  });
  assert(value == 800);
  printf("%s: OK\n", name);
}

void testThreadPool() {
  ThreadPool pool(4);
  std::vector<std::future<long long>> results;
  for (int i = 0; i < 2000; i++)
    results.push_back(
        pool.submit([](long long a, long long b) { return a * b; }, i, i + 1));
  for (int i = 0; i < 2000; i++)
    assert(results[i].get() == (long long)i * (i + 1));
  // an exception thrown by a task reaches the caller through its future
  auto failing = pool.submit([]() -> int { throw std::runtime_error("boom"); });
  bool caught = false;
  try {
    (void)failing.get();
  } catch (const std::runtime_error &e) {
    caught = std::string(e.what()) == "boom";
  }
  assert(caught);
  puts("ThreadPool: OK");
}

// every meal happens, and neighbours never eat at the same time
template <class D> void testDining(const char *name) {
  // static: each class keeps its forks for the whole run. libstdc++'s
  // std::mutex has no destructor ThreadSanitizer sees, so a later object in the
  // same memory would inherit the earlier one's lock order
  static D d;
  std::atomic<int> eating[5] = {};
  std::atomic<int> meals{0};
  inThreads(5, [&](int p) {
    for (int i = 0; i < 300; i++)
      d.wants_to_eat(
          p, [] {}, [] {},
          [&] {
            ++eating[p];
            assert(eating[(p + 1) % 5] == 0 && eating[(p + 4) % 5] == 0);
            ++meals;
            --eating[p];
          },
          [] {}, [] {});
  });
  assert(meals == 1500);
  printf("%s: OK\n", name);
}

int main() {
  testMutex();
  testSemaphore<Semaphore>("Semaphore");
  testSemaphore<SemaphorePrimitive>("SemaphorePrimitive");
  testProducerConsumer<ProducerConsumerCV>("ProducerConsumerCV");
  testProducerConsumer<ProducerConsumer>("ProducerConsumer");
  testReaderWriter<ReaderWriter>(
      "ReaderWriter", &ReaderWriter::reader_lock, &ReaderWriter::reader_unlock,
      &ReaderWriter::writer_lock, &ReaderWriter::writer_unlock);
  testReaderWriter<ReaderWriterCV>("ReaderWriterCV", &ReaderWriterCV::read_lock,
                                   &ReaderWriterCV::read_unlock,
                                   &ReaderWriterCV::write_lock,
                                   &ReaderWriterCV::write_unlock);
  testThreadPool();
  testDining<DiningPhilosophersOrder>("DiningPhilosophersOrder");
  testDining<DiningPhilosophersSemaphore>("DiningPhilosophersSemaphore");
  testDining<DiningPhilosophersCV>("DiningPhilosophersCV");
  puts("os: OK");
}
