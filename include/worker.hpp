#pragma once

#include <queue>

namespace yah {
  template<class T>
  class Worker {
    public:
      pthread_mutex_t               mutex;
      pthread_cond_t                cond;

      unsigned                      num_workers;
      std::queue<T>                 data;

      Worker(unsigned num_workers);
      virtual void push(const T&);
      void raise();
      virtual T pop();
  };

  template<class T>
  Worker::Worker(unsigned num_workers) {
    this->num_workers = num_workers;
    this->data = std::queue<T>();
    
  }
}
