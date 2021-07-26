#pragma once

// 所有的全局变量

#include <string>

#include "config.hpp"
#include "log.hpp"
#include "cache.hpp"
#include "db.hpp"

namespace yah {
  extern Config config;
  extern Log log;

  extern Endl endl;
  extern Success success;
  extern Warn warn;
  extern Fatal fatal;
  extern Time ctime;

  extern DB db;

  extern Cache<std::string> ap_cache;
  extern Cache<std::string> apstation_cache;

  extern std::string device_name;
  extern pid_t airodump_pid;
}
