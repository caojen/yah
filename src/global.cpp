#include "global.hpp"

namespace yah {
  Config config;
  Log log;

  Endl endl;
  Success success;
  Warn warn;
  Fatal fatal;
  Time ctime;

  DB db;

  Cache<std::string> ap_cache;
  Cache<std::string> apstation_cache;

  std::string device_name;
  pid_t airodump_pid;

  ThreadPool* formatter;
  ThreadPool* checker;
  ThreadPool* sender;
}
