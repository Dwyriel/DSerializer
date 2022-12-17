cmake_minimum_required(VERSION 3.24)
project(JsonParser)
project(execs)

set(CMAKE_CXX_STANDARD 17)

add_library(JsonParser SHARED JsonParser.cpp)

add_executable(exec main.cpp)