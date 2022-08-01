#include "checker.hpp"
#include "global.hpp"

namespace yah {
  static inline time_t to_next_time(time_t t) {
    struct tm* tm = localtime(&t);
    if (tm->tm_sec < 55) {
      if (tm->tm_sec % 10 <= 5) {
        tm->tm_sec = tm->tm_sec / 10 * 10 + 5;
      } else {
        tm->tm_sec = tm->tm_sec / 10 * 10 + 10;
      }
    } else {
      tm->tm_sec = 0;
      tm->tm_min += 1;
    }

    return mktime(tm);
  }

  Checker::Checker(std::unique_ptr<AirodumpData>& ptr) {
    this->ptr = std::move(ptr);
  }

  void Checker::run() {

    auto limit = 0;
    if(this->ptr->is_ap()) {
      limit = config.ap_pwr_limit;
    } else if (this->ptr->is_apstation()) {
      limit = config.ap_station_pwr_limit;
    } else {
      return;
    }

    printf("this->ptr->get_pwr() is %d, where limit is %d\n", this->ptr->get_pwr(), limit);

    if (this->ptr->get_pwr() >= limit) {
      auto time = this->ptr->get_create_time();
      this->ptr->set_create_time(to_next_time(time));
      printf("ok, pass to cache and db\n");

      if(!this->ptr->in_cache() && !this->ptr->in_db()) {
        this->ptr->sync_db();
        printf("ok, pass to sender\n");
        // 持久化完成，交给sender
        sender->push(std::move(this->ptr));
      }
    }
  }
}
