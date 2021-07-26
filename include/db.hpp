#pragma once

#include <sqlite3.h>
#include <pthread.h>

#include <string>

namespace yah {
  class DB {
    public:
      std::string           path;   // db的物理路径
      sqlite3*              db;     // db对象指针
      pthread_mutex_t       mutex;  // db的写锁

      DB();
      DB(std::string path);
      ~DB();
  };
}
