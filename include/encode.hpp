#pragma once

#include <string>
#include <cstring>

namespace yah {
  class Encode {
    public:
      Encode(const std::string& text);

      std::string encode() const;
    private:
      std::string text;
  };
}
