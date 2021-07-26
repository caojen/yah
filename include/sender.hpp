#pragma once

#include "thread_pool.hpp"
#include "airodump.hpp"
#include "global.hpp"

namespace yah {
  class Sender: public Job {
    public:

      std::unique_ptr<AirodumpData> ptr;

      Sender(std::unique_ptr<AirodumpData>& ptr);
      void run();
  };
}
