#include "checker.hpp"
#include "global.hpp"

namespace yah {
  Checker::Checker(std::unique_ptr<AirodumpData>& ptr) {
    this->ptr = std::move(ptr);
  }

  void Checker::run() {
    if(!this->ptr->in_cache() && !this->ptr->in_db()) {
      this->ptr->sync_db();
      // 持久化完成，交给sender
      sender->push(std::move(this->ptr));
    }
  }
}
