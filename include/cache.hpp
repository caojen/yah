#pragma once

#include <string>
#include <time.h>
#include <map>
#include <list>
#include <queue>
#include <vector>

namespace yah {
  typedef unsigned long HashKey;

  class Hash {
    public:
      HashKey operator()(const std::string&) const;
  };

  extern Hash hash;

  template<class T>
  class Cache {
    struct Item {
      HashKey     key;
      time_t      create_time;
      T           data;
    };

    unsigned                            max = 0;
    unsigned                            cur = 0;
    unsigned                            timeout = 0;
    std::map<HashKey, std::list<Item>>  pool;
    
    public:
      Cache(unsigned max = 0, unsigned timeout = 0);
      // 如果当前缓存中不存在data，那么插入这个值，然后返回true
      // 否则，返回false
      bool insert(const T& data);
  };

  template<class T>
  Cache<T>::Cache(unsigned max, unsigned timeout) {
    this->max = max;
    this->timeout = timeout;
  }

  template<class T>
  bool Cache<T>::insert(const T& data) {
    time_t now = time(NULL);
    HashKey hashKey = hash(data);

    auto iter = pool.find(hashKey);
    if(iter != pool.end()) {
      // HashKey存在，检查
      std::list<Item>& list = iter->second;
      auto it = list.begin();
      while(it != list.end()) {
        if(now - it->create_time >= this->timeout) {
          it = list.erase(it);
        } else if(it->data == data) {
          it->create_time = now;
          return false;
        } else {
          ++it;
        }
      }
    }
    // 决定插入这个data
    // 检查缓存是否超出限制
    if(this->cur >= this->max) {
      // 超出限制，删除长度最多的那个
      unsigned maxlen = 0;
      auto hashKey = pool.begin();
      for(auto it = pool.begin(); it != pool.end(); ++it) {
        if(it->second.size() > maxlen) {
          hashKey = it;
          maxlen = it->second.size();
        }
      }
      cur -= maxlen;
      pool.erase(hashKey);
    } 
    // 插入这个data
    pool[hashKey].push_back(Item {hashKey, now, data});
    return true;
  }
}
