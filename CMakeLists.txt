cmake_minimum_required(VERSION 3.10)
project(doxide
    VERSION 0.0.0
    DESCRIPTION "Modern documentation for modern C++"
    HOMEPAGE_URL "https://doxide.org")

set(CMAKE_CXX_STANDARD 20)
if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS YAML_DECLARE_STATIC)
endif()
include_directories(
    src
    contrib/tree-sitter/lib/include
    contrib/glob/single_include
)
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
    src/Entity.cpp
)
target_link_libraries(doxide yaml)

include(GNUInstallDirs)
install(TARGETS doxide RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/nsis ${CMAKE_MODULE_PATH})
string(REPLACE "/" "\\\\" NATIVE_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(CPACK_PACKAGE_NAME "Doxide")
set(CPACK_PACKAGE_ICON "${NATIVE_SOURCE_DIR}\\\\nsis\\\\icon.ico")
set(CPACK_PACKAGE_VENDOR "Lawrence Murray")
set(CPACK_PACKAGE_DESCRIPTION "Modern documentation for modern C++")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern documentation for modern C++")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-installer")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME}")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
set(CPACK_NSIS_MODIFY_PATH ON)
set(CPACK_NSIS_MANIFEST_DPI_AWARE ON)
set(CPACK_NSIS_IGNORE_LICENSE_PAGE ON)
set(CPACK_NSIS_MUI_ICON "${NATIVE_SOURCE_DIR}\\\\nsis\\\\icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${NATIVE_SOURCE_DIR}\\\\nsis\\\\icon.ico")
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${NATIVE_SOURCE_DIR}\\\\nsis\\\\welcome2x.bmp")
set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${NATIVE_SOURCE_DIR}\\\\nsis\\\\welcome2x.bmp")
set(CPACK_NSIS_MUI_HEADERIMAGE "${NATIVE_SOURCE_DIR}\\\\nsis\\\\header2x.bmp")
include(CPack)
