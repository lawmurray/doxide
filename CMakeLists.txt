cmake_minimum_required(VERSION 3.10)
project(doxide)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(src)
add_compile_options(-g)
add_library(libyaml SHARED IMPORTED)
add_library(libtree-sitter SHARED IMPORTED)
add_executable(doxide
    contrib/tree-sitter-cpp/src/parser.c
    contrib/tree-sitter-cpp/src/scanner.cc
    src/doxide.cpp
    src/cpp/CppParser.cpp
    src/doc/DocTokenizer.cpp
    src/doc/DocTranslator.cpp
    src/markdown/MarkdownGenerator.cpp
    src/yaml/YAMLNode.cpp
    src/yaml/YAMLParser.cpp
    src/Driver.cpp
    src/Entity.cpp)
target_link_libraries(doxide yaml tree-sitter)
