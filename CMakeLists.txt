cmake_minimum_required(VERSION 3.10)
project(doxide
    VERSION 0.9.0
    DESCRIPTION "Modern documentation for modern C++"
    HOMEPAGE_URL "https://doxide.org"
)
cmake_policy(SET CMP0074 NEW) # use <PackageName>_ROOT to look for packages

set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Available options to control how dependencies are sourced
# - submodule: use only git submodules for dependencies (legacy behavior)
# - target: look for CMake targets, if not found error
# - auto: (default) first look for CMake targets, if not found, use git submodules
set(DEPS_STRATEGY_VALUES "auto;submodule;target")
option(DEPS_SOURCE_STRATEGY "How to source dependencies; possible values are ${DEPS_STRATEGY_VALUES}" OFF)

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

if(DEPS_STRATEGY STREQUAL "auto" OR DEPS_STRATEGY STREQUAL "target")
    set(DEPS_STRATEGY_FIND_PACKAGE ON)
endif()

if(DEPS_STRATEGY STREQUAL "target")
    set(FORCED_REQUIRE REQUIRED)
endif()

if(NOT yaml_FOUND AND DEPS_STRATEGY_FIND_PACKAGE)
    find_package(yaml CONFIG ${FORCED_REQUIRE})
endif()
if(NOT yaml_FOUND AND NOT DEPS_STRATEGY STREQUAL "submodule")
    # use Findyaml.cmake module provided and find in system
    find_package(yaml MODULE)
endif()
if(NOT yaml_FOUND)
    message(STATUS "Sourcing yaml from submodule")
    add_subdirectory(contrib/libyaml EXCLUDE_FROM_ALL SYSTEM)
endif()

# convenience macro to source a dependency: first look for a CMake target (if allowed)
# then source it from the given files in the submodule
macro(find_dep_dual_mode dep_name VAR val1)
    if(NOT ${dep_name}_FOUND AND DEPS_STRATEGY_FIND_PACKAGE)
        find_package(${dep_name} CONFIG ${FORCED_REQUIRE})
    endif()
    if(${dep_name}_FOUND)
        list(APPEND DEPS_TARGETS ${dep_name}::${dep_name})
    else()
        set(__the_list ${ARGV})
        if(${ARGC} GREATER 3)
            list(SUBLIST __the_list 3 -1 __file_args)
        endif()
        message(STATUS "Sourcing ${dep_name} from submodule")
        list(APPEND ${VAR} ${val1} ${__file_args})
        unset(__file_args)
    endif()
endmacro()

find_dep_dual_mode(tree-sitter-cuda DEPS_SOURCES
    contrib/tree-sitter/lib/src/lib.c contrib/tree-sitter-cuda/src/parser.c contrib/tree-sitter-cuda/src/scanner.c
)
if(NOT tree-sitter-cuda_FOUND)
    list(APPEND DEPS_INCLUDE_DIRS contrib/tree-sitter/lib/include)
endif()

find_dep_dual_mode(CLI11 DEPS_INCLUDE_DIRS contrib/CLI11/include)
find_dep_dual_mode(p-ranav-glob DEPS_INCLUDE_DIRS contrib/glob/single_include)

add_executable(doxide
    ${DEPS_SOURCES} # sources of the submodules (if any)
    src/doxide.cpp
    src/CppParser.cpp
    src/Doc.cpp
    src/DocToken.cpp
    src/DocTokenizer.cpp
    src/Driver.cpp
    src/Entity.cpp
    src/GcovCounter.cpp
    src/JSONCounter.cpp
    src/JSONGenerator.cpp
    src/MarkdownGenerator.cpp
    src/SourceWatcher.cpp
    src/YAMLNode.cpp
    src/YAMLParser.cpp
)
if(WIN32)
    # add icon
    target_sources(doxide PRIVATE win/icon.rc)
endif()
target_include_directories(doxide PRIVATE
    src
    ${DEPS_INCLUDE_DIRS}
)

target_link_libraries(doxide yaml ${DEPS_TARGETS})

configure_file(
    ${CMAKE_SOURCE_DIR}/src/config.h.in
    ${CMAKE_SOURCE_DIR}/src/config.h
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
