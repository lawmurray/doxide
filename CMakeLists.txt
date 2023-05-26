cmake_minimum_required(VERSION 3.10)
project(doxide)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(src)
add_library(libyaml SHARED IMPORTED)
add_library(libtree-sitter SHARED IMPORTED)
add_executable(doxide
    contrib/tree-sitter-cpp/src/parser.c
    contrib/tree-sitter-cpp/src/scanner.cc
    src/doxide.cpp
    src/parser/Language.cpp
    src/parser/Parser.cpp
    src/parser/Tokenizer.cpp
    src/markdown/MarkdownGenerator.cpp
    src/yaml/YAMLNode.cpp
    src/yaml/YAMLParser.cpp
    src/Driver.cpp
    src/Entity.cpp)
target_link_libraries(doxide yaml tree-sitter)

include(GNUInstallDirs)
install(TARGETS doxide)
