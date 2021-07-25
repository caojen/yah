#include "global.hpp"

namespace yah {
  Config config;
  Log log;

  Endl endl;
  Success success;
  Warn warn;
  Fatal fatal;
  Time time;

  Cache<std::string> ap_cache;
  Cache<std::string> apstation_cache;

  Formatter formatter;
  std::string device_name;
}
