#include "core.hpp"
#include "thread_pool.hpp"
#include "move.hpp"

namespace yah {
  static inline void init_formatter() {
    std::function<std::string(std::string&)> f = static_cast<std::string(*)(std::string&)>(move);
    formatter = ThreadPool<std::string>("Formatter",
      config.num_formatter,
      f
    );
  }
  static inline void init_checker();
  static inline void init_sender();
}

namespace yah {
  void core_start() {
    init_formatter();
  }
}
