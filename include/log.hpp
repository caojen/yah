#pragma once

#include <iostream>

namespace yah {
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
      return "[Time] ";
    }
  };
}
