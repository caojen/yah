#pragma once

#include <iostream>

namespace yah {
  class Log {
    public:
      template<class T>
      Log& operator<<(const T& stream) {
        std::cout << stream << std::endl;
        return *this;
      }
  };
}
