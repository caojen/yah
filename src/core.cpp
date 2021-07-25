#include <unistd.h>
#include <string>

#include "core.hpp"
#include "move.hpp"
#include "global.hpp"

namespace yah {
  static inline void init_ap_cache() {
    ap_cache = Cache<std::string>(config.num_cache_max, config.ap_cache_timeout);
  }
  static inline void init_apstation_cache() {
    apstation_cache = Cache<std::string>(config.num_cache_max, config.apstation_cache_timeout);
  }
  static inline void init_formatter();
  static inline void init_checker();
  static inline void init_sender();
}

namespace yah {
  void core_start() {
    init_ap_cache();
    init_apstation_cache();
  }
}
