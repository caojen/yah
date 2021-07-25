#pragma once

// 所有的全局变量

#include <string>

#include "config.hpp"
#include "log.hpp"
#include "cache.hpp"
#include "formatter.hpp"

namespace yah {
  extern Config config;
  extern Log log;

  extern Endl endl;
  extern Success success;
  extern Warn warn;
  extern Fatal fatal;
  extern Time time;

  extern Cache<std::string> ap_cache;
  extern Cache<std::string> apstation_cache;

  extern Formatter formatter;
  extern std::string device_name;
}
