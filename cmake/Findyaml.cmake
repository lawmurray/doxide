
find_path(YAML_INCLUDE_DIR
    NAMES yaml.h
    PATH_SUFFIXES include
    DOC "YAML header"
)

# the user may have injected a custom yaml_ROOT to drive package discovery
if(NOT yaml_ROOT OR NOT YAML_ROOT)
    # if not, infer yaml_ROOT from YAML_INCLUDE_DIR;
    # rationale: depending on the compiler (e.g., system compiler or installed
    # via conda/pixi/etc.) some custom paths may be inferred from the compiler
    # and included, while the header in YAML_INCLUDE_DIR belongs to a different
    # path; for example:
    # - YAML_INCLUDE_DIR /usr/include/yaml.h
    # - YAML_LIBRARY <project dir>/.pixi/envs/default/lib/libyaml.so
    # the solution is then to start the search from the base path of YAML_INCLUDE_DIR
    get_filename_component(BASE_PATH ${YAML_INCLUDE_DIR} DIRECTORY)
    set(yaml_ROOT ${BASE_PATH})
endif()

find_library(YAML_LIBRARY
    NAMES yaml
    PATH_SUFFIXES ${CMAKE_SYSTEM_PROCESSOR}-linux-gnu
    DOC "YAML library"
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(yaml
    REQUIRED_VARS YAML_LIBRARY YAML_INCLUDE_DIR
)

if(yaml_FOUND)
    mark_as_advanced(YAML_LIBRARY YAML_INCLUDE_DIR)
    add_library (yaml UNKNOWN IMPORTED)

    # check whether it's a static library
    string(REGEX MATCH "\.(a|lib)$" _EXTENSION ${YAML_LIBRARY})
    # setting usage requirements, as from its CMake infra
    # https://github.com/yaml/libyaml/blob/840b65c40675e2d06bf40405ad3f12dec7f35923/CMakeLists.txt#L71-L72
    set(_TARGET_DEFS "$<$<BOOL:${MSVC}>:_CRT_SECURE_NO_WARNINGS>")
    if (_EXTENSION STREQUAL ".a" OR _EXTENSION STREQUAL ".lib")
        list(APPEND _TARGET_DEFS YAML_DECLARE_STATIC)
    endif()

    set_target_properties(yaml
        PROPERTIES
        IMPORTED_LOCATION ${YAML_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${YAML_INCLUDE_DIR}
        INTERFACE_COMPILE_DEFINITIONS "${_TARGET_DEFS}"
    )
endif()
