cmake_minimum_required(VERSION 3.0)

project (yah)

set(CMAKE_CXX_STANDARD 11)

AUX_SOURCE_DIRECTORY(src SRC_LIST)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

include_directories(include)

find_package(Threads)
find_package(ZLIB)
add_compile_options(-Wall -g)

add_executable(yah ${SRC_LIST} base/yah.cpp)
target_link_libraries(yah pthread)
target_link_libraries(yah sqlite3)
target_link_libraries(yah z)
target_link_libraries(yah OpenSSL::SSL)

add_executable(btl base/btl.cpp)
target_link_libraries(btl pthread)
target_link_libraries(btl sqlite3)
target_link_libraries(btl z)
target_link_libraries(btl OpenSSL::SSL)
