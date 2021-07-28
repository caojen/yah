#include "formatter.hpp"
#include "checker.hpp"

namespace yah {
  Formatter::Formatter(std::string& s) {
    this->line = std::move(s);
  }

  void Formatter::run() {
    auto airodump = AirodumpData::init_origin_line(this->line);
    if(airodump != nullptr) {
      std::unique_ptr<AirodumpData> ptr(airodump);
      // 格式化完成，将这个ptr交给checker
      std::unique_ptr<Job> c(new Checker(ptr));
      
      checker->push(c);
    }
  }
}