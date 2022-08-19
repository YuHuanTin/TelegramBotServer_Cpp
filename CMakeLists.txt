cmake_minimum_required(VERSION 3.22)
project(TGapi)

set(CMAKE_CXX_STANDARD 17)

include_directories(openssl/debug/include)

add_executable(TGapi DataTypes.h LinuxServerWithLibEvent.cpp)