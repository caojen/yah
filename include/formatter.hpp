#pragma once

#include <queue>
#include <string>
#include <pthread.h>

namespace yah {

  class Formatter {
    public:
      pthread_mutex_t               mutex;
      pthread_cond_t                cond;

      std::queue<std::string>       lines;
      unsigned                      num_workers;
      
      Formatter();
      Formatter(unsigned num_workers);

      void push(const std::string& s);
      void raise();
      std::string pop();
  };

  void* formatter_do(void* __f);
}
