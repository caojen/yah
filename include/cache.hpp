#pragma once

#include <string>
#include <time.h>
#include <map>
#include <list>

namespace yah {
  typedef unsigned long HashKey;

  class Hash {
    public:
      HashKey operator()(const std::string&) const;
  };

  extern Hash hash;

  template<class T>
  class Cache {
    class Item {
      HashKey     key;
      time_t      create_time;
      T           data;
    };

    unsigned                max;
    unsigned                cur;
    unsigned                timeout;
    std::map<HashKey, std::list<Item>> pool;
    
    public:
      Cache(unsigned max, unsigned timeout);
      // 如果当前缓存中不存在data，那么插入这个值，然后返回true
      // 否则，返回false
      bool insert(const T& data);
  };

  template<class T>

}
