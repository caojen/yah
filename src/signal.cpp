#include <signal.h>
#include "global.hpp"
#include "signal.hpp"

namespace yah {
  static void sighup_handler(int signo, siginfo_t* info, void* context) {
    if(signo == SIGHUP) {
      config.reload();
    }
  }

  void SignalHandler::handle() {
    log << ctime << "[SignalHandler]" << endl;
    auto signals = { SIGHUP };
    auto handlers = { sighup_handler };
    struct sigaction sa;

    for(auto i = 0; i < handlers.size(); i++) {
      sa.sa_sigaction = *(handlers.begin() + i);
      sa.sa_flags = SA_SIGINFO;
      sigemptyset(&sa.sa_mask);
      sigaddset(&sa.sa_mask, *(signals.begin() + i));
      if(sigaction(*(signals.begin() + i), &sa, NULL) < 0) {
        log << ctime << warn << "[SignalHandler] " << "Cannot register signal handler at " << i << endl;
      }
    }
  }
}
