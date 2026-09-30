/**
 * std::invoke_result_t<F, Args...> gives return type
 * std::packaged_task<R()> wraps a callable R()
 * std::bind(f, args...) binds arguments to callable
 */

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool {
private:
  bool stop;
  std::mutex mtx;
  std::condition_variable cv;
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

public:
  ThreadPool(std::size_t numThreads) : stop(false) {
    for (std::size_t i = 0; i < numThreads; ++i) {
      workers.emplace_back([this]() {
        while (true) {
          std::function<void()> task;
          {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [this]() { return stop || !tasks.empty(); });
            if (stop && tasks.empty())
              return;
            task = std::move(tasks.front());
            tasks.pop();
          }
          try {
            task();
          } catch (...) {
            // Handle exceptions thrown by tasks if necessary
          }
        }
      });
    }
  }

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;

  template <typename F, typename... Args>
  auto submit(F &&f, Args &&...args)
      -> std::future<std::invoke_result_t<F, Args...>> {

    using return_type = std::invoke_result_t<F, Args...>;
    auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task_ptr->get_future();
    {
      std::unique_lock<std::mutex> lk(mtx);
      if (stop)
        throw std::runtime_error("submit on stopped ThreadPool");
      tasks.emplace([task_ptr]() { (*task_ptr)(); });
    }
    cv.notify_one();
    return res;
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lk(mtx);
      stop = true;
    }
    cv.notify_all();
    for (auto &worker : workers)
      if (worker.joinable())
        worker.join();
  }
};