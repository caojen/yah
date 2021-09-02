#pragma once

#include <list>
#include <mutex>

namespace yah {
  class WhiteList {
  public:
    WhiteList();
    bool has_data(const std::string& s);

  private:
    std::list<std::string>     white_data;
    std::mutex                 mutex;
  };

  extern WhiteList* whiteList;
}
