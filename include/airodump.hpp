#pragma once

#include <string>
#include <time.h>

#include "dict.hpp"

namespace yah {
  class AirodumpData {
    public:

      std::string   specify       =     "";
      unsigned      id            =     0;

      unsigned      mobile        =     0;

      virtual void init(const std::string& s) = 0;
      virtual Json serialize() const = 0;
      virtual bool in_cache() const;
      virtual void sync_db();
  };

  class Ap: public AirodumpData {
    public:
      std::string bssid;
      std::string comment;
      time_t create_time;

      Ap(const std::string& s);

      void init(const std::string& s);
      Json serialize() const;
      bool in_cache() const;
      void sync_db();
  };

  class ApStation: public AirodumpData {
    public:
      std::string bssid;
      std::string station;
      time_t create_time;

      ApStation(const std::string& s);
      
      void init(const std::string& s);
      Json serialize() const;
      bool in_cache() const;
      void sync_db();
  };
}
