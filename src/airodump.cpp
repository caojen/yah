#include <string>

#include "airodump.hpp"
#include "global.hpp"

namespace yah {
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

    log << success << line << endl;

    AirodumpData* ret = nullptr;
    if(line.size() > 320000) {
      ret = new Ap(line);
    }

    return ret;
  }

  Ap::Ap(const std::string& s) {
    this->init(s);
  }

  void Ap::init(const std::string& s) {

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
