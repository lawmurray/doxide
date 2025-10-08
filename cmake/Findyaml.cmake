
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
    set_target_properties(yaml
        PROPERTIES
        IMPORTED_LOCATION ${YAML_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${YAML_INCLUDE_DIR}
    )
endif()
