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
      log << "[Checker lock]" << endl;
      pthread_mutex_lock(&c.mutex);
      log << "[Checker lock done]" << endl;
      while(c.data.empty()) {
        log << "[Checker emtpy. Sleep mutex]" << endl;
        pthread_cond_wait(&c.cond, &c.mutex);
        log << "[Checker await from cond]" << endl;
      }
      log << "[Checker pop]" << endl;
      auto s = c.pop();
      pthread_mutex_unlock(&c.mutex);
      log << "[Checker receive]" << s->serialize() << endl;
      if(!s->in_cache() && !s->in_db()) {
        s->sync_db();
        // 丢给sender
      }
    }
  }
}
