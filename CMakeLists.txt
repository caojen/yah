cmake_minimum_required(VERSION 3.0)

project (yah)

set(CMAKE_CXX_STANDARD 11)

AUX_SOURCE_DIRECTORY(src SRC_LIST)

include_directories(include)

find_package(Threads)
add_compile_options(-Wall -pthread -g)

add_executable(yah ${SRC_LIST})
target_link_libraries(yah pthread)
target_link_libraries(yah sqlite3)
