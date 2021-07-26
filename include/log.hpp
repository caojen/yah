#pragma once

#include <iostream>
#include <time.h>

namespace yah {
  static inline std::string parse_log_time(time_t t) {
    char r[32] = { 0 };
    struct tm* tm = localtime(&t);
    strftime(r, 32, "%Y-%m-%d %H:%M:%S", tm);
    return std::string(r);
  }

  class Log {
    public:
      template<class T>
      Log& operator<<(const T& stream) {
        std::cout << stream;
        return *this;
      }
  };

  class LogSpecial {
    public:
      friend Log& operator<<(Log& log, const LogSpecial& s) {
        log << s.str();
        return log;
      }
      friend std::ostream& operator<<(std::ostream& o, const LogSpecial& s) {
        o << s.str();
        return o;
      }
      virtual std::string str() const = 0;
  };

  class Endl: public LogSpecial {
    virtual std::string str() const {
      return std::string("\n");
    }
  };

  class Success: public LogSpecial {
    virtual std::string str() const {
      return "[Okay] ";
    }
  };

  class Warn: public LogSpecial {
    virtual std::string str() const {
      return "[Warn] ";
    }
  };

  class Fatal: public LogSpecial {
    virtual std::string str() const {
      return "[Fatal] ";
    }
  };

  class Time: public LogSpecial {
    virtual std::string str() const {
      std::string ret = "";
      ret.append("[");
      ret += parse_log_time(time(NULL));
      ret.append("] ");
      return ret;
    }
  };
}
