#include <condition_variable>
#include <mutex>
#include <semaphore>

class ReaderWriter {
private:
  int readers;
  std::mutex mtx;
  std::binary_semaphore sem;

public:
  ReaderWriter() : readers(0), sem(1) {}

  void reader_lock() {
    std::lock_guard lk(mtx);
    if (++readers == 1)
      sem.acquire();
  }

  void reader_unlock() {
    std::lock_guard lk(mtx);
    if (!--readers)
      sem.release();
  }

  void writer_lock() { sem.acquire(); }

  void writer_unlock() { sem.release(); }
};

class ReaderWriterCV {
private:
  int reader_count;
  int waiting_writers;
  bool writer_active;
  std::mutex mtx;
  std::condition_variable reader_cv;
  std::condition_variable writer_cv;

public:
  ReaderWriterCV()
      : reader_count(0), waiting_writers(0), writer_active(false) {}

  void read_lock() {
    std::unique_lock lk(mtx);
    reader_cv.wait(lk, [&]() { return !writer_active && !waiting_writers; });
    reader_count++;
  }

  void read_unlock() {
    std::unique_lock lk(mtx);
    if (!--reader_count)
      writer_cv.notify_one();
  }

  void write_lock() {
    std::unique_lock lk(mtx);
    ++waiting_writers;
    writer_cv.wait(lk, [&]() { return !reader_count && !writer_active; });
    --waiting_writers;
    writer_active = true;
  }

  void write_unlock() {
    std::unique_lock lk(mtx);
    writer_active = false;
    if (waiting_writers > 0)
      writer_cv.notify_one();
    else
      reader_cv.notify_all();
  }
};