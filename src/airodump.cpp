#include <string>
#include <time.h>
#include <sqlite3.h>

#include "airodump.hpp"
#include "global.hpp"
#include "white_list.hpp"

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

  static inline std::string parse_time(time_t t) {
    char r[32] = { 0 };
    struct tm* tm = localtime(&t);
    strftime(r, 32, "%Y-%m-%d %H:%M:%S", tm);
    return std::string(r);
  }

  AirodumpData::AirodumpData(std::string& remote): remote(remote) {}

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

  Ap::Ap(): AirodumpData(config.remote_ap) {}

  Ap::Ap(const std::string& s): AirodumpData(config.remote_ap) {
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
    this->mobile  = config.device;
  }

  Json Ap::serialize() const {
    auto ret = Json(Json::JsonType::DICT);
    ret.set("id", this->id);
    ret.set("mobile", this->mobile);
    ret.set("bssid", this->bssid);
    ret.set("comment", this->comment);
    ret.set("create_time", parse_time(this->create_time));
    ret.set("etl_time", parse_time(time(NULL)));
    return ret;
  }

  bool Ap::in_cache() const {
    return ap_cache.insert(this->specify);
  }

  bool Ap::in_db() const {
    const std::string& specify = this->specify;
    bool in_white_list = whiteList->has_data(specify);
    unsigned timeout = in_white_list ? 5 : config.ap_cache_timeout;

    bool is_in = false;
    sqlite3* s = db.db;

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(s, "SELECT count(1) AS count FROM ap WHERE bssid = ? AND create_time > ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, specify.c_str(), specify.size(), SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, this->create_time - timeout);
    int ret = sqlite3_step(stmt);
    if(ret == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if(count > 0) {
            is_in = true;
        }
    } else if(ret == SQLITE_DONE) {
        is_in = true;
    }
    sqlite3_finalize(stmt);

    return is_in;
  }

  void Ap::sync_db() {
    char* sql = sqlite3_mprintf("INSERT INTO ap(bssid, comment, create_time) VALUES('%q', '%q', %d)", this->bssid.c_str(), this->comment.c_str(), this->create_time);
    db.lock();
    sqlite3_exec(db.db, sql, NULL, NULL, NULL);
    this->id = sqlite3_last_insert_rowid(db.db);
    db.unlock();
  }

  Ap::~Ap() {
    // log << "[ApDeleted]" << endl;
  }

  ApStation::ApStation(): AirodumpData(config.remote_apstation) {}

  ApStation::ApStation(const std::string& s): AirodumpData(config.remote_apstation) {
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
    this->mobile  = config.device;
  }

  Json ApStation::serialize() const {
    auto ret = Json(Json::JsonType::DICT);
    ret.set("id", this->id);
    ret.set("mobile", this->mobile);
    ret.set("bssid", this->bssid);
    ret.set("station", this->station);
    ret.set("comment", this->comment);
    ret.set("create_time", parse_time(this->create_time));
    ret.set("etl_time", parse_time(time(NULL)));

    return ret;
  }

  bool ApStation::in_cache() const {
    return apstation_cache.insert(this->specify);
  }

  bool ApStation::in_db() const {
    const std::string& specify = this->specify;
    bool in_white_list = whiteList->has_data(specify);
    unsigned timeout = in_white_list ? 5 : config.apstation_cache_timeout;

    bool is_in = false;
    sqlite3* s = db.db;

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(s, "SELECT count(1) AS count FROM apstation WHERE station = ? AND create_time > ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, specify.c_str(), specify.size(), SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, this->create_time - timeout);
    int ret = sqlite3_step(stmt);
    if(ret == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if(count > 0) {
            is_in = true;
        }
    } else if(ret == SQLITE_DONE) {
        is_in = true;
    }
    sqlite3_finalize(stmt);

    return is_in;
  }

  void ApStation::sync_db() {
    char* sql = sqlite3_mprintf("INSERT INTO apstation(bssid, station, comment, create_time) VALUES('%q', '%q', '%q', %d)", this->bssid.c_str(), this->station.c_str(), this->comment.c_str(), this->create_time);
    db.lock();
    sqlite3_exec(db.db, sql, NULL, NULL, NULL);
    this->id = sqlite3_last_insert_rowid(db.db);
    db.unlock();
  }

  ApStation::~ApStation() {
    // log << "[ApStationDeleted]" << endl;
  }
}
