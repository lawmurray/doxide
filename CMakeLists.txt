cmake_minimum_required(VERSION 3.10)
project(doxide)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(src)
add_compile_options(-g)
add_library(libyaml SHARED IMPORTED)
add_executable(doxide
    src/doxide.hpp
    src/doxide.cpp
    src/doxide/Config.hpp
    src/doxide/Config.cpp
    src/doxide/Driver.hpp
    src/doxide/Driver.cpp
    src/doxide/Token.hpp
    src/doxide/Token.cpp
    src/doxide/Tokenizer.hpp
    src/doxide/Tokenizer.cpp)
target_link_libraries(doxide yaml)
