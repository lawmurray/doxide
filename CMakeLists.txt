cmake_minimum_required(VERSION 3.10)
project(doxide
    VERSION 0.0.0
    DESCRIPTION "Modern documentation for modern C++"
    HOMEPAGE_URL "https://doxide.org")
set(CMAKE_CXX_STANDARD 20)
include_directories(src contrib/tree-sitter/lib/include)
add_library(libyaml SHARED IMPORTED)
add_executable(doxide
    contrib/tree-sitter/lib/src/lib.c
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
target_link_libraries(doxide yaml)
include(GNUInstallDirs)
install(TARGETS doxide RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
