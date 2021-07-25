#pragma once

#include <string>
#include <iostream>
#include <functional>
#include <queue>
#include <vector>
#include <pthread.h>

#include "log.hpp"
#include "global.hpp"
#include "move.hpp"

namespace yah {
  extern Log log;
  extern Endl endl;
}

namespace yah {
  template <class T>
  class ThreadPool;
  
  template <class T>
  static void* thread_pool_main(void* __thread_pool) {
    ThreadPool<T>* pool = (ThreadPool<T>*)__thread_pool;
    // 从pool的queue中获取一个job
    while(1) {
      pthread_mutex_lock(&pool->mutex);
      while(pool->queue.size() == 0) {
        pthread_cond_wait(&pool->cond, &pool->mutex);
      }
      T job = pool->move(pool->queue.front()); pool->queue.pop();
      pthread_mutex_unlock(&pool->mutex);
      pool->job(job);
    }
    return NULL;
  }
}

namespace yah {
  template<class T>
  class ThreadPool: public LogSpecial {

    public:

      std::string desc;
      std::string s;

      unsigned num_worker;
      unsigned num_job;

      std::queue<T> queue;
      pthread_mutex_t mutex;
      pthread_cond_t cond;

      std::vector<pthread_t> workers;

      std::function<void(T&)> job;  // 指明对于T，需要做的内容
      std::function<T(T&)> move;  // 指明如何从queue中的元素复制出来

      ThreadPool();
      ThreadPool(
        std::string desc,
        unsigned num_worker,
        // std::function<void(T&)> job,
        std::function<T(T&)> __move
      );

      virtual std::string str() const;

      void thread_pool_init_thread();
      
      void push(T job);
  };
}

namespace yah {
  template<class T>
  ThreadPool<T>::ThreadPool() {}

  template<class T>
  ThreadPool<T>::ThreadPool(
    std::string desc,
    unsigned num_worker,
    // std::function<void(T&)> job,
    std::function<T(T&)> move
  ) {
    this->desc = desc;
    this->s = "[";
    this->s += desc + "] ";

    this->num_worker = num_worker;
    this->move = move;

    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->cond, NULL);

    this->thread_pool_init_thread();
  }

  template<class T>
  std::string ThreadPool<T>::str() const {
    return this->s;
  }

  template<class T>
  void ThreadPool<T>::thread_pool_init_thread() {
    // 新建num_thread个线程，每个线程的主函数是thread_pool_main
    for(unsigned i = 0; i < this->num_worker; i++) {
      pthread_t pid;
      pthread_create(&pid, NULL, thread_pool_main<T>, this);
    }
  }

  template<class T>
  void ThreadPool<T>::push(T job) {
    this->queue.push(job);
  }
}
