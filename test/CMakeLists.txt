cmake_minimum_required(VERSION 3.24)
project(DSerializerTest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(DSerializer STATIC ../DSerializer.cpp)
add_library(DVariant STATIC ../libs/DVariant/DVariant.cpp)
add_executable(DSerializerTest main.cpp)

include_directories(../)#in place of target_include_directories for 'DSerializer' and 'DSerializerTest's

target_link_libraries(DSerializer DVariant)
target_link_libraries(DSerializerTest DSerializer)
