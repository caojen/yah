#pragma once

#include <string>
#include "log.hpp"

// 程序start时进行检查
// 成功时返回true

namespace yah {

  class Check: public LogSpecial {
    public:
      virtual std::string str() const {
        return std::string("[Checking] ");
      }
  };

  extern Check check;

  bool precheck();

  bool check_is_root();

  bool check_airodump();

  bool check_lockfile();

  bool check_wlan();

  void check_init_sql();

  unsigned check_get_locking_pid();

  int get_airodump_device_name(char name[64]);

}
