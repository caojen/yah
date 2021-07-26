#include <chrono>

#include "thread_pool.hpp"

namespace yah {
  ThreadPool::ThreadPool() {}

  ThreadPool::ThreadPool(size_t num_workers) {
    for(size_t i = 0; i < num_workers; i++) {
      std::thread thread([this]() {
        auto pid = std::this_thread::get_id();
        while(1) {
          std::unique_lock<std::mutex> lk(this->mutex);
          cv.wait(lk, [this] { return !this->jobs.empty(); });
          auto job = std::move(this->jobs.front());
          this->jobs.pop();
          job->run();
        }
      });
      thread.detach();
    }
  }

  void ThreadPool::push(std::unique_ptr<Job>& job) {
    std::lock_guard<std::mutex> lk(this->mutex);
    this->jobs.push(std::move(job));
    this->cv.notify_one();
  }
}
