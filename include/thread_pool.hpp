#pragma once

#include <string>
#include <iostream>
#include <functional>
#include <queue>
#include <vector>
#include <pthread.h>

#include "log.hpp"
#include "global.hpp"

namespace yah {
  template<class T>
  class ThreadPool: public LogSpecial {

    std::string desc;
    std::string s;

    unsigned num_worker;
    unsigned num_job;

    std::queue<T> queue;
    pthread_mutex_t mutex;

    std::vector<pthread_t> workers;

    std::function<void(T&)> job;

    public:
      ThreadPool();
      ThreadPool(std::string desc, unsigned num_worker);

      virtual std::string str() const;

      void thread_pool_init_thread();
      void* thread_pool_main(void* null);

  };
}

namespace yah {
  template<class T>
  ThreadPool<T>::ThreadPool() {}

  template<class T>
  ThreadPool<T>::ThreadPool(std::string desc, unsigned num_worker) {
    this->desc = desc;
    this->s = "[";
    this->s += desc + "] ";

    this->num_worker = num_worker;
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
      pthread_create(&pid, NULL, this->thread_pool_main, NULL);
      log << *this << "create pthread at " << pid << endl;
    }
  }

  template<class T>
  void* ThreadPool<T>::thread_pool_main(void* null) {
    log << *this << "thread pool main" << endl;
    return NULL;
  }
}