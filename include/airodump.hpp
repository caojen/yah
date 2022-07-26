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
      virtual bool is_ap() const = 0;
      virtual bool is_apstation() const = 0;
      virtual unsigned int get_pwr() const = 0;
      virtual time_t get_create_time() const = 0;
      virtual void set_create_time(time_t time) = 0;

      AirodumpData(std::string& remote);
      virtual ~AirodumpData();

      static AirodumpData* init_origin_line(const std::string& s);
  };

  class Ap: public AirodumpData {
    public:
      std::string bssid;
      std::string comment;
      time_t create_time;

      Ap();
      Ap(const std::string& s);

      void init(const std::string& s);
      Json serialize() const;
      bool in_cache() const;
      bool in_db() const;
      void sync_db();
      bool is_ap() const { return true; }
      bool is_apstation() const { return false; }
      unsigned int get_pwr() const;
      time_t get_create_time() const;
      void set_create_time(time_t time);

      ~Ap();
  };

  class ApStation: public AirodumpData {
    public:
      std::string bssid;
      std::string station;
      std::string comment;
      time_t create_time;

      ApStation();
      ApStation(const std::string& s);
      
      void init(const std::string& s);
      Json serialize() const;
      bool in_cache() const;
      bool in_db() const;
      void sync_db();
      bool is_ap() const { return false; }
      bool is_apstation() const { return true; }
      unsigned int get_pwr() const;
      time_t get_create_time() const;
      void set_create_time(time_t time);

      ~ApStation();
  };
}
