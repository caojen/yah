#include <unistd.h>
#include <string>

#include "core.hpp"
#include "move.hpp"
#include "global.hpp"
#include "yah_pty.h"

namespace yah {
  static inline void init_ap_cache() {
    ap_cache = Cache<std::string>(config.num_cache_max, config.ap_cache_timeout);
  }
  static inline void init_apstation_cache() {
    apstation_cache = Cache<std::string>(config.num_cache_max, config.apstation_cache_timeout);
  }
  static inline void init_formatter() {
    formatter = Formatter(config.num_formatter);
  }
  static inline void init_checker();
  static inline void init_sender();
}

namespace yah {
  void core_start() {
    init_ap_cache();
    init_apstation_cache();
    init_formatter();

    int airodump_fd = run_airodump();
  }

  int run_airodump() {
    int fd = 0;
    char slave_name[MAX_PTYNAME];
    struct winsize wsize;
    wsize.ws_row = YAH_PTY_ROWS;
    wsize.ws_col = YAH_PTY_COLS;

    pid_t pid = yah_pty_fork(&fd, slave_name, MAX_PTYNAME, NULL, &wsize);

    if(pid == 0) {
      // 子进程
      execl(config.airodump_path, config.airodump_name, device_name.c_str(), "-C", "2412-2472,5180-5825", "-f", "10", "--berlin", "3", NULL);
    } else {
      return fd;
    }
  }
}
