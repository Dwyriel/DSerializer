cmake_minimum_required(VERSION 3.24)
project(DSerializer)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(DSerializer SHARED DSerializer.cpp)

target_link_libraries(DSerializer DVariant)
