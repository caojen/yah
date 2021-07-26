#include <string>
#include <time.h>

#include "airodump.hpp"
#include "global.hpp"

namespace yah {


  static inline bool substr(const std::string& s, const std::string& pattern) {
    unsigned len = pattern.size();
    if(s.size() < pattern.size()) {
      return false;
    }

    return s.substr(0, len) == pattern;
  }

  static inline std::string::const_iterator get_part_end(const std::string::const_iterator& iter) {

    auto ret = iter;

    if(*ret == '(') {
      while(*ret != ' ') ++ret;
      while(*ret == ' ') ++ret;
    }

    while(*ret != ' ') ++ret;
    return ret;
  }

  static inline std::string::const_iterator get_part_begin(const std::string& s, unsigned part) {
    auto iter = s.begin();
    while(part--) {
      iter = get_part_end(iter);
      while(iter != s.end() && *iter == ' ') {
        ++iter;
      }
    }

    return iter;
  }

  AirodumpData::~AirodumpData() {}

  AirodumpData* AirodumpData::init_origin_line(const std::string& s) {
    std::string line = "";

    bool heading = true;
    for(auto it = s.begin(); it != s.end();) {
      if(*it == ESC) {
        while(it != s.end() && *it != ' ') {
          ++it;
        }
      }

      while(it != s.end() && heading && *it == ' ') {
        ++it;
      }

      if(it != s.end()) {
        heading = false;
        line.append(std::string(1, *it));
        ++it;
      }
    }

    // log << success << line << endl;

    AirodumpData* ret = nullptr;
    // 检查line的开头是不是不需要的字段:
    const std::string excepts[] = {
      "Freq", "BSSID", "available", 
      "failed:", "ioctl(SIOCGIFINDEX)"
    };

    for(auto &e: excepts) {
      if(substr(line, e)) {
        goto output;
      }
    }

    // 检查line的长度是否大于20
    if(line.size() > 20) {
      // 可以接受的line，通过判断第二个部分的长度，或第一部分是否为(not associated)，来决定是ap还是apstation
      if(substr(line, "(not associated)")) {
        // apstation
        ret = new ApStation(line);
      } else {
        auto iter = line.begin();
        while(iter != line.end() && *iter != ' ') ++iter;
        while(iter != line.end() && *iter == ' ') ++iter;
        auto ptr = iter;
        while(ptr != line.end() && *ptr != ' ') ++ptr;
        unsigned second_len = ptr - iter;
        if(second_len > 15) {
          // apstation
          ret = new ApStation(line);
        } else {
          // ap
          ret = new Ap(line);
        }
      }
    }
output:
    return ret;
  }

  Ap::Ap(const std::string& s) {
    this->init(s);
  }

  void Ap::init(const std::string& s) {
    auto first_begin = get_part_begin(s, 0);
    auto first_end   = get_part_end(first_begin);
    const unsigned len = first_end - first_begin;
    this->bssid = s.substr(first_begin - s.begin(), len);
    this->comment = s;
    this->create_time = time(NULL);

    this->specify = this->bssid;
  }

  Json Ap::serialize() const {
    return Json();
  }

  bool Ap::in_cache() const {
    return false;
  }

  void Ap::sync_db() {

  }

  ApStation::ApStation(const std::string& s) {
    this->init(s);
  }

  void ApStation::init(const std::string& s) {
    auto first_begin = get_part_begin(s, 0);
    auto first_end   = get_part_end(first_begin);
    unsigned len = first_end - first_begin;
    this->bssid = s.substr(first_begin - s.begin(), len);

    auto second_begin = get_part_begin(s, 1);
    auto second_end = get_part_end(second_begin);
    len = second_end - second_begin;
    this->station = s.substr(second_begin - s.begin(), len);

    this->comment = s;
    this->create_time = time(NULL);

    this->specify = this->station;
  }

  Json ApStation::serialize() const {
    return Json();
  }

  bool ApStation::in_cache() const {
    return false;
  }

  void ApStation::sync_db() {

  }
}