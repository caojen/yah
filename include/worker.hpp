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

      Worker();
      Worker(unsigned num_workers);
      virtual void push(T&);
      void raise();
      virtual T pop();
  };

  template<class T>
  Worker<T>::Worker() {}

  template<class T>
  Worker<T>::Worker(unsigned num_workers) {
    this->num_workers = num_workers;
    this->data = std::queue<T>();

    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->cond, NULL);
    
    // 由于worker需要使用的函数不确定，这里交给基类完成
  }

  template<class T>
  void Worker<T>::push(T& t) {
    this->data.push(std::move(t));
  }

  template<class T>
  void Worker<T>::raise() {
    pthread_cond_signal(&this->cond);
  }

  template<class T>
  T Worker<T>::pop() {
    T ret = move(this->data.front());
    this->data.pop();
    return move(ret);
  }
}
