#include <iostream>
#include <assert.h>

#include "dict.hpp"
#include "config.hpp"
#include "global.hpp"

int main(int argc, char** argv) {
  std::cout << "Hello World" << std::endl;

  yah::Dict dict("abc", "def");
  std::cout << dict << std::endl;
  std::vector<yah::Dict> vec;
  vec.push_back(dict);
  std::cout << yah::Dict::serialize(vec) << std::endl;

  yah::config = yah::Config("../yah.conf");

  yah::log << "ok, logging...";
}
