#pragma once

#include <queue>
#include <string>
#include <pthread.h>

#include "worker.hpp"
#include "checker.hpp"

namespace yah {

  class Formatter: public Worker<std::string> {
    public:
      Formatter();
      Formatter(unsigned num_workers);
  };

  void* formatter_do(void* __f);
}
