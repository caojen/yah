#include <signal.h>
#include "global.hpp"
#include "signal.hpp"

namespace yah {
  static void sighup_handler(int signo, siginfo_t* info, void* context) {
    if(signo == SIGHUP) {
      config.reload();
    }
  }

  static void sigchld_handler(int signo, siginfo_t* info, void* context) {
    pid_t child = info->si_pid;
    log << ctime << "[SIGCHLD] Receive From Child Pid = " << child << endl;
    log << ctime << "[SIGCHLD] Current Airodump Child Pid is " << airodump_pid << endl;
    if(airodump_pid != 0 && airodump_pid == child) {
      log << ctime << "[SIGCHLD] Matched. It seems that Airodump-ng has exited. Abort.";
      abort();
    }
  }

  static void sigterm_handler(int signo, siginfo_t* info, void* context) {
    log << ctime << "[SIGTERM] Received. Tring to abort." << endl;
    log << ctime << "[SIGTERM] Current Airodump Child Pid is " << airodump_pid << endl;
    kill(airodump_pid, SIGTERM);
    log << ctime << "[SIGTERM] Sent SIGTERM to Child " << airodump_pid << endl;
    log << ctime << "[SIGTERM] Aborted." << endl;
  }

  void SignalHandler::handle() {
    log << ctime << "[SignalHandler]" << endl;
    auto signals = { SIGHUP, SIGCHLD, SIGTERM };
    auto handlers = { sighup_handler, sigchld_handler, sigterm_handler };
    struct sigaction sa;

    for(unsigned i = 0; i < handlers.size(); i++) {
      sa.sa_sigaction = *(handlers.begin() + i);
      sa.sa_flags = SA_SIGINFO;
      sigemptyset(&sa.sa_mask);
      sigaddset(&sa.sa_mask, *(signals.begin() + i));
      if(sigaction(*(signals.begin() + i), &sa, NULL) < 0) {
        log << ctime << warn << "[SignalHandler] " << "Cannot register signal handler at " << i << endl;
      }
    }
    log << ctime << success << "[SignalHandler] Done" << endl;
  }
}
