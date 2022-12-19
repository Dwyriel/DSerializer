cmake_minimum_required(VERSION 3.24)
project(JsonParser)
project(JsonParserTest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(JsonParser SHARED JsonParser.cpp)

add_library(DVariant SHARED libs/DVariant/DVariant.cpp)

target_link_libraries(JsonParser DVariant)

add_executable(JsonParserTest main.cpp)

target_link_libraries(JsonParserTest JsonParser)