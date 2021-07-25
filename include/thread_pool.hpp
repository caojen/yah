#pragma once

#include <string>
#include <iostream>
#include <functional>

#include "log.hpp"

namespace yah {
  template<class T>
  class ThreadPool: public LogSpecial {

    std::string desc;
    std::string s;

    unsigned num_thread;
    unsigned num_job;

    public:
      ThradPool();
      ThreadPool(std::string desc, unsigned num_thread, unsigned num_job);

      virtual std::string str() const;

      void thread_pool_main();

  };
}

namespace yah {
  template<class T>
  ThreadPool<T>::ThreadPool(std::string desc, unsigned num_thread, unsigned num_job) {
    this->desc = desc;
    this->s = "[";
    this->s += desc + "] ";

    this->num_thread = num_thread;
    this->num_job = num_job;

    this->thread_pool_main();
  }

  template<class T>
  std::string ThreadPool<T>::str() const {
    return this->s;
  }

  template<class T>
  void ThreadPool<T>::thread_pool_main() {

  }
}