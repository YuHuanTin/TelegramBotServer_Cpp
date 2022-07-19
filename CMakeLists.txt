cmake_minimum_required(VERSION 3.22)
project(TGapi)

set(CMAKE_CXX_STANDARD 17)

#预处理器定义
add_definitions(-D BUILDING_LIBCURL)
include_directories(openssl/debug/include)

add_executable(TGapi main.cpp DataTypes.h)