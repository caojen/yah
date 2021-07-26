#pragma once

#include <string>
#include <time.h>

#include "dict.hpp"

namespace yah {

  const unsigned ESC = 27;

  class AirodumpData {
    public:

      std::string   specify       =     "";
      unsigned      id            =     0;

      unsigned      mobile        =     0;
      std::string&  remote;

      virtual void init(const std::string& s) = 0;
      virtual Json serialize() const = 0;
      virtual bool in_cache() const = 0;
      virtual bool in_db() const = 0;
      virtual void sync_db() = 0;

      AirodumpData(std::string& remote);
      virtual ~AirodumpData();

      static AirodumpData* init_origin_line(const std::string& s);
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
      bool in_db() const;
      void sync_db();

      ~Ap();
  };

  class ApStation: public AirodumpData {
    public:
      std::string bssid;
      std::string station;
      std::string comment;
      time_t create_time;

      ApStation(const std::string& s);
      
      void init(const std::string& s);
      Json serialize() const;
      bool in_cache() const;
      bool in_db() const;
      void sync_db();

      ~ApStation();
  };
}
