cmake_minimum_required(VERSION 3.10)
project(doxide)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(src)
add_library(libyaml SHARED IMPORTED)
add_executable(doxide
    src/doxide.hpp
    src/doxide.cpp
    src/doxide/Config.hpp
    src/doxide/Config.cpp
    src/doxide/Driver.hpp
    src/doxide/Driver.cpp)
target_link_libraries(doxide yaml)
