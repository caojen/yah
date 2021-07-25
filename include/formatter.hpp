#pragma once

#include <queue>
#include <string>
#include <pthread.h>

namespace yah {

  void* formatter_do(Formatter* f);

  class Formatter {
    public:
      pthread_mutex_t               mutex;
      pthread_cond_t                cond;

      std::queue<std::string>       lines;
      unsigned                      num_workers;
      
      Formatter();
      Formatter(unsigned num_workers);

      void push(std::string& s);
      std::string pop();
  };
}
