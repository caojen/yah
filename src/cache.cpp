#include "cache.hpp"

namespace yah {
  HashKey Hash::operator()(const std::string& s) const {
    HashKey hash = 5381;
    for(auto &ch: s) {
      hash = ((hash << 5) + hash) + ch;
    }

    return hash;
  }

  Hash hash;
}
