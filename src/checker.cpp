#include <memory>
#include "pthread.h"

#include "checker.hpp"
#include "global.hpp"


namespace yah {
  Checker::Checker() {}

  Checker::Checker(unsigned num_workers)
    : Worker(num_workers) {
      pthread_t pid;
      for(unsigned i = 0; i < num_workers; i++) {
        pthread_create(&pid, NULL, checker_do, NULL);
      }

      log << success << "Checker init done, with size = " << num_workers << endl;
  }

  void* checker_do(void* __f) {
    Checker& c = checker;

    while(1) {
      log << "[Checker] lock" << endl;
      pthread_mutex_lock(&c.mutex);
      log << "[Checker] lock done" << endl;
      while(c.data.empty()) {
        log << "[Checker] sleep" << endl;
        pthread_cond_wait(&c.cond, &c.mutex);
        log << "[Checker] await" << endl;
      }
      log << "[Checker] pop" << endl;
      auto s = c.pop();
      pthread_mutex_unlock(&c.mutex);
      log << "[Checker] " << s->serialize() << endl;
      // 在这个时候，s的id总是0，表示这个数据总是新采集的
      // 原有的数据，应该直接发送到sender中，因为原有的数据不需要进行checker的判断了
      if(!s->in_cache()) {
        log << "[Checker] not in cache" << endl;
        if(!s->in_db()) {
          log << "[Checker] not in db" << endl;
          s->sync_db();
          log << "[Checker] " << "write to db with id" << s->id << endl;
        } else {
          log << "[Checker] in db" << endl;
        }
      } else {
        log << "[Checker] in cache" << endl;
      }
    }
  }
}
