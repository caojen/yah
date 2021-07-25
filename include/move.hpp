#pragma once

// 指明如何进行move
#include <memory>
#include <string>

namespace yah {
  std::string move(std::string& s);

  template<class T>
  std::unique_ptr<T> move(std::unique_ptr<T>& s) {
    return s.move();
  }
}
