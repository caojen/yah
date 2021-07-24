cmake_minimum_required(VERSION 3.21)

project (yah)

set(CMAKE_CXX_STANDARD 11)

AUX_SOURCE_DIRECTORY(src SRC_LIST)

include_directories(include)

add_compile_options(-Wall -lpthread)

add_executable(yah ${SRC_LIST})
