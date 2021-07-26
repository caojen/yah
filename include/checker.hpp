#pragma once

#include "thread_pool.hpp"
#include "airodump.hpp"
#include "global.hpp"

namespace yah {
  class Checker: public Job {
    public:
      std::unique_ptr<AirodumpData> ptr;

      Checker(std::unique_ptr<AirodumpData>& ptr);

      void run();
  };
}
