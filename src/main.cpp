#include <iostream>
#include <assert.h>
#include <string.h>

#include "dict.hpp"
#include "config.hpp"
#include "global.hpp"
#include "check.hpp"
#include "core.hpp"

int main(int argc, char** argv) {
  if (argc < 2) {
    goto usage;
  }

  if(!strcmp(argv[1], "start")) {
    std::string config_file = argv[2];
    yah::config = yah::Config(config_file);
    yah::Config::show(yah::config);
    
    if(yah::precheck() == false) {
      goto exit;
    }

    yah::core_start();

    goto unreachable;

  } else if(!strcmp(argv[1], "reload")) {

  } else {
    goto usage;
  }


usage:
  printf("Usage: %s start {config_file} | reload\n", argv[0]);
exit:
  return 1;
unreachable:
  printf("Program run into unreachable code.\n");
  return 2;
}