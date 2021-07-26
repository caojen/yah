#include <memory>

#include <unistd.h>

#include "formatter.hpp"
#include "airodump.hpp"
#include "global.hpp"

namespace yah {
  Formatter::Formatter() {}

  Formatter::Formatter(unsigned num_workers)
    : Worker(num_workers) {
    // 创建workers线程
    pthread_t pid;
    for(unsigned i = 0; i < num_workers; i++) {
      pthread_create(&pid, NULL, formatter_do, NULL);
    }

    log << success << "Formatter init done, with size = " << this->num_workers << endl;
  }

  void* formatter_do(void* __f) {
    Formatter& f = formatter;
    while(1) {
      pthread_mutex_lock(&f.mutex);
      while(f.data.empty()) {
        pthread_cond_wait(&f.cond, &f.mutex);
      }
      std::string s = f.pop();
      pthread_mutex_unlock(&f.mutex);
      auto airodump = AirodumpData::init_origin_line(s);
      
      if(airodump != nullptr) {
        log << "[Formatter] " << airodump->serialize() << endl;
        std::unique_ptr<AirodumpData> ptr(airodump);
        // 将这个指针丢给checker
        pthread_mutex_lock(&checker.mutex);
        checker.push(ptr);
        pthread_mutex_unlock(&checker.mutex);
        checker.raise();
      }
    }

    return NULL;
  }
}
