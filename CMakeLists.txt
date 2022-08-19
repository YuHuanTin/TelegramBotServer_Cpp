cmake_minimum_required(VERSION 3.22)
project(TGapi)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(TGapi WinServerWithLibEvent/DataTypes.h WinServerWithLibEvent/WinServerWithLibEvent.cpp WinServerWithLibEvent/WinServerWithLibEvent.h)