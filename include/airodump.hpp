#pragma once

#include <string>

#include "dict.hpp"

namespace yah {
  class AirodumpData {
    public:

      std::string   specify       =     "";
      unsigned      id            =     0;

      unsigned      mobile        =     0;

      virtual void init(const std::string& s) = 0;
      // virtual Dict serialize() const = 0;
  }

  class Ap: public AirodumpData {
    
  }
}
