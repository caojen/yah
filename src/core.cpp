#include "core.hpp"
#include "thread_pool.hpp"

namespace yah {
  static inline void init_formatter() {
    formatter = ThreadPool<std::string>("Formatter", config.num_formatter);
  }
  static inline void init_checker();
  static inline void init_sender();
}

namespace yah {
  void core_start() {
    init_formatter();
  }
}
