#pragma once

// 一个典型的线程池
// 线程池包含两个部分，一个是 num_workers 个工作线程
// 另外一个是一个队列，队列中包含的元素是 T 类型的，并且，类型T需要实现 run 方法，在这里我们用基类Job代替T
// 线程池将获得Job的所有权，因此，使用unique_ptr来保存数据

#include <thread>
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace yah {
  class Job;

  class ThreadPool {
    public:
      // 什么都不做的构造函数
      ThreadPool();

      // 指明工作线程数量
      // 当线程池构造完成后，所有线程自动被构建，并且进行工作
      ThreadPool(size_t num_workers);

      // 将一个工作推入到线程中
      void push(std::unique_ptr<Job>& job);
    
    private:
      // 含有所有任务的队列
      std::queue<std::unique_ptr<Job>>        jobs;
      // 多线程控制互斥锁
      std::mutex                              mutex;
      // 条件变量
      std::condition_variable                 cv;
  };

  class Job {
    public:
      virtual void run() = 0;
      virtual ~Job() {}
  };
}
