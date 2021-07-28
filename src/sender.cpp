#include "sender.hpp"

namespace yah {
  Sender::Sender(std::unique_ptr<AirodumpData>& ptr) {
    this->ptr = std::move(ptr);
  }

  void Sender::run() {
    log << "[Sender] " << this->ptr->serialize() << endl;
  }
}
