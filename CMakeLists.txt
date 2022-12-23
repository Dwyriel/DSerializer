cmake_minimum_required(VERSION 3.24)
project(DSerializer)
project(DSerializerTest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(DSerializer STATIC DSerializer.cpp)

add_library(DVariant STATIC libs/DVariant/DVariant.cpp)

target_link_libraries(DSerializer DVariant)

add_executable(DSerializerTest main.cpp)

target_link_libraries(DSerializerTest DSerializer)