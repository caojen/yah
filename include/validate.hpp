#pragma once

namespace yah {
  class Validate {
  public:
    Validate() noexcept;
    static void check();

  private:
    static void fatal();
  };

  extern Validate validate;
}
