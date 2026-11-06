cmake_minimum_required(VERSION 3.10)
project(doxide
  VERSION 0.10.0
  DESCRIPTION "Modern documentation for modern C++"
  HOMEPAGE_URL "https://doxide.org"
)
cmake_policy(SET CMP0074 NEW) # use <PackageName>_ROOT to look for packages

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Available options to control how dependencies are sourced
# - submodule: use only git submodules for dependencies (legacy behavior)
# - target: look for CMake targets, if not found error
# - auto: (default) first look for CMake targets, if not found, use git submodules
set(DEPS_STRATEGY_VALUES "auto;submodule;target")
option(DEPS_SOURCE_STRATEGY "How to source dependencies;\
 possible values are ${DEPS_STRATEGY_VALUES}" OFF)

if(NOT DEPS_SOURCE_STRATEGY)
  set(DEPS_STRATEGY "auto")
else()
  if(NOT ${DEPS_SOURCE_STRATEGY} IN_LIST DEPS_STRATEGY_VALUES)
    message(FATAL_ERROR "the value of DEPS_SOURCE_STRATEGY \
(${DEPS_SOURCE_STRATEGY}) is not one of ${DEPS_STRATEGY_VALUES}"
      )
  endif()
  set(DEPS_STRATEGY ${DEPS_SOURCE_STRATEGY})
endif()

# source dependency either from CMake target (if available) or from submodule
function(source_dependency name type)
  if(NOT ${name}_FOUND AND NOT DEPS_STRATEGY STREQUAL "submodule")
    find_package(${name} CONFIG QUIET)
    if(NOT ${name}_FOUND)
      find_package(${name} MODULE QUIET)
    endif()
  endif()
  if(${name}_FOUND)
    if(NOT TARGET ${name}::${name})
      add_library(${name}::${name} ALIAS ${name})
    endif()
    message(STATUS "Sourced \"${name}\" from target.")
    return()
  elseif(DEPS_STRATEGY STREQUAL "target")
    # only target is selected, if not found err out
    message(FATAL_ERROR "Cannot find an existing dependency \"${name}\"")
  endif()
  # otherwise go on and try with submodule

  set(multiValueArgs INTERFACE LIBRARY SOURCES)
  set(oneValueArgs SUBDIR)
  cmake_parse_arguments(PARSE_ARGV 1 parsed
    "" "${oneValueArgs}" "${multiValueArgs}"
  )
  if(type STREQUAL "SUBDIR")
    message(STATUS "parsed_SUBDIR IS ${parsed_SUBDIR}")
    add_subdirectory(contrib/${parsed_SUBDIR} EXCLUDE_FROM_ALL SYSTEM)
  elseif(type STREQUAL "INTERFACE")
    list(TRANSFORM parsed_INTERFACE PREPEND "contrib/")
    add_library(${name} INTERFACE)
    target_include_directories(${name} SYSTEM INTERFACE ${parsed_INTERFACE})
  elseif(type STREQUAL "LIBRARY")
    list(TRANSFORM parsed_LIBRARY PREPEND "contrib/")
    list(TRANSFORM parsed_SOURCES PREPEND "contrib/")
    add_library(${name} STATIC EXCLUDE_FROM_ALL ${parsed_SOURCES})
    target_include_directories(${name} SYSTEM PUBLIC ${parsed_LIBRARY})
  else()
    message(FATAL_ERROR "Dependency type \"${type}\" not known.")
  endif()
  add_library(${name}::${name} ALIAS ${name})
  message(STATUS "Sourced \"${name}\" from submodule.")
endfunction()

source_dependency(yaml SUBDIR libyaml)
source_dependency(tree-sitter-cuda LIBRARY tree-sitter/lib/include
  SOURCES tree-sitter/lib/src/lib.c tree-sitter-cuda/src/parser.c tree-sitter-cuda/src/scanner.c
)
source_dependency(CLI11 INTERFACE CLI11/include)
source_dependency(p-ranav-glob INTERFACE glob/single_include)

if(TARGET tree-sitter-cuda)
  # the dependency was sourced from submodule: use sources directly
  get_target_property(src_files tree-sitter-cuda SOURCES)
  list(TRANSFORM src_files PREPEND "${PROJECT_SOURCE_DIR}/")
  get_target_property(inc_dirs tree-sitter-cuda INTERFACE_INCLUDE_DIRECTORIES)
else()
  set(link_deps LINK_LIBRARIES tree-sitter-cuda::tree-sitter-cuda)
endif()

try_compile(COMPILE_RESULT
  SOURCES ${PROJECT_SOURCE_DIR}/cmake/extract_ts_ids.cpp ${src_files}
  CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${PROJECT_SOURCE_DIR}/src;${inc_dirs}"
  ${link_deps}
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED TRUE
  OUTPUT_VARIABLE COMPILE_LOG
  COPY_FILE "${CMAKE_BINARY_DIR}/generate/extract_ts_ids${CMAKE_EXECUTABLE_SUFFIX}"
)
if(NOT COMPILE_RESULT)
  message(FATAL_ERROR "Compilation failed with code '${COMPILE_RESULT}'"
  " and log\n--------\n${COMPILE_LOG}\n--------\n")
endif()

foreach(VAR IN ITEMS QueryCppNodes QueryCppExcludeNodes QueryCppIncludeNodes)
  execute_process(
    COMMAND "${CMAKE_BINARY_DIR}/generate/extract_ts_ids${CMAKE_EXECUTABLE_SUFFIX}" ${VAR}
      RESULT_VARIABLE RUN_RESULT
      OUTPUT_VARIABLE ${VAR}_Code
  )
  if(RUN_RESULT)
    message(FATAL_ERROR "Run failed with code '${RUN_RESULT}'"
      " and stdout\n--------\n${${VAR}_Code}\n--------\n")
  endif()
endforeach()

configure_file(
  ${CMAKE_SOURCE_DIR}/src/cpp_query_enums.hpp.in
  ${CMAKE_BINARY_DIR}/src/generated/cpp_query_enums.hpp
)
configure_file(
  ${CMAKE_SOURCE_DIR}/src/config.h.in
  ${CMAKE_BINARY_DIR}/src/generated/config.h
)

add_executable(doxide
  src/doxide.cpp
  src/CppParser.cpp
  src/Driver.cpp
  src/DriverConfigParser.cpp
  src/FileSystemDeleter.cpp
  src/FileSystemStatsInfo.cpp
  src/GcovCounter.cpp
  src/JSONCounter.cpp
  src/JSONGenerator.cpp
  src/SourceWatcher.cpp
  src/StyleOption.cpp
  src/TextLineCursor.cpp
  src/Utils.cpp
  src/YAMLNode.cpp
  src/YAMLParser.cpp
  src/doc_parser/DocToken.cpp
  src/doc_parser/DocTokenizer.cpp
  src/doc_parser/StyleParsers.cpp
  src/generation/DocRenderers.cpp
  src/generation/MarkdownGenerator.cpp
  src/generation/MdCoverageGenerator.cpp
  src/generation/Sanitize.cpp
)
if(WIN32)
  # add icon
  target_sources(doxide PRIVATE win/icon.rc)
endif()
target_include_directories(doxide PRIVATE src ${CMAKE_BINARY_DIR}/src)

target_link_libraries(doxide yaml::yaml tree-sitter-cuda::tree-sitter-cuda
  CLI11::CLI11 p-ranav-glob::p-ranav-glob
)

include(GNUInstallDirs)
install(TARGETS doxide RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/win ${CMAKE_MODULE_PATH})
string(REPLACE "/" "\\\\" NATIVE_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
set(CPACK_PACKAGE_NAME "Doxide")
set(CPACK_PACKAGE_ICON "${NATIVE_SOURCE_DIR}\\\\win\\\\icon.ico")
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
set(CPACK_NSIS_MUI_ICON "${NATIVE_SOURCE_DIR}\\\\win\\\\icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${NATIVE_SOURCE_DIR}\\\\win\\\\icon.ico")
set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${NATIVE_SOURCE_DIR}\\\\win\\\\welcome2x.bmp")
set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${NATIVE_SOURCE_DIR}\\\\win\\\\welcome2x.bmp")
set(CPACK_NSIS_MUI_HEADERIMAGE "${NATIVE_SOURCE_DIR}\\\\win\\\\header2x.bmp")
include(CPack)
