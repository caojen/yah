#pragma once

#include <memory>

#include "worker.hpp"
#include "airodump.hpp"

namespace yah {
  class Checker: public Worker<std::unique_ptr<AirodumpData>> {
    public:
      Checker();
      Checker(unsigned num_workers);
  };

  void* checker_do(void* __f);
}
