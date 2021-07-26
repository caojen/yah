#pragma once

#include "thread_pool.hpp"
#include "airodump.hpp"
#include "global.hpp"

namespace yah {
  class Formatter: public Job {
    public:

      std::string line;

      Formatter(std::string& s);
      void run();
      
  };
}
