#pragma once

// 所有的全局变量

#include "config.hpp"
#include "log.hpp"

namespace yah {
  extern Config config;
  extern Log log;

  extern Endl endl;
  extern Success success;
  extern Warn warn;
  extern Fatal fatal;
  extern Time time;
}
