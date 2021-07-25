#include <memory>

#include "formatter.hpp"
#include "airodump.hpp"

namespace yah {
  Formatter::Formatter() {}

  Formatter::Formatter(unsigned num_workers) {
    this->num_workers = num_workers;

    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->cond, NULL);

    // 创建workers
    for(unsigned i = 0; i < num_workers; i++) {

    }
  }

  void Formatter::push(const std::string& s) {
    this->lines.push(s);
  }

  void Formatter::raise() {
    pthread_cond_signal(&this->cond);
  }

  std::string Formatter::pop() {
    std::string ret = this->lines.front();
    this->lines.pop();
    return ret;
  }

  void* formatter_do(Formatter* f) {
    while(1) {
      pthread_mutex_lock(&f->mutex);
      while(f->lines.empty()) {
        pthread_cond_wait(&f->cond, &f->mutex);
      }

      std::string s = f->pop();
      pthread_mutex_unlock(&f->mutex);

      auto airodump = AirodumpData::init_origin_line(s);
      if(airodump != nullptr) {
        std::unique_ptr<AirodumpData> ptr(airodump);
      }
    }

    return NULL;
  }
}
