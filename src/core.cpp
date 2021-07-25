#include <unistd.h>
#include <string>

#include "core.hpp"
#include "move.hpp"

namespace yah {
  static inline void init_formatter();
  static inline void init_checker();
  static inline void init_sender();
}

namespace yah {
  void core_start() {}
}
