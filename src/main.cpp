#include <signal.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>

#include "dict.hpp"
#include "config.hpp"
#include "global.hpp"
#include "check.hpp"
#include "core.hpp"
#include "signal.hpp"

void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

int main(int argc, char** argv) {
  if (argc < 2) {
    goto usage;
  }

  signal(SIGSEGV, handler);
  signal(SIGABRT, handler);

  if(!strcmp(argv[1], "start")) {
    if(argc < 3) {
      goto usage;
    }
    yah::log << yah::ctime << "Program starting..." << yah::endl;
    
    std::string config_file = argv[2];
    yah::config = yah::Config(config_file);
    
    if(yah::precheck() == false) {
      goto exit;
    }

    // 注册信号
    yah::SignalHandler::handle();

    yah::core_start();

    goto unreachable;

  } else if(!strcmp(argv[1], "reload")) {
    yah::log << yah::ctime << "Reload" << yah::endl;

    if(yah::check_is_root() == false) {
      yah::log << yah::ctime << "You should be root." << yah::endl;
      abort();
    }
    // 获得当前正在运行的进程
    yah::log << yah::ctime << "Check Lockfile..." << yah::endl;
    if(yah::check_lockfile() == false) {
      yah::log << yah::ctime << yah::success << yah::endl;
      auto pid = yah::check_get_locking_pid();
      yah::log << yah::ctime << yah::success << "Get Pid = " << pid << yah::endl;
      yah::log << yah::ctime << "Send SIGHUP" << yah::endl;
      kill(pid, SIGHUP);
    } else {
      yah::log << yah::fatal << "It Seems that no process is running. Abort." << yah::endl;
      abort();
    }

    return 0;
  } else {
    goto usage;
  }


usage:
  printf("Usage: %s start {config_file} | reload\n", argv[0]);
exit:
  return 1;
unreachable:
  printf("Program run into unreachable code.\n");
  abort();
}
