#pragma once

#include <string>
#include <regex>
#include <stack>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <variant>

#include <cassert>

#include <yaml.h>
#include <tree_sitter/api.h>
#include <glob/glob.hpp>
#include <CLI/CLI.hpp>

/**
 * Print a warning message.
 * 
 * @ingroup developer
 */
#define warn(msg) \
    std::cerr << "warning: " << msg << std::endl;

/**
 * Print an error message and exit.
 * 
 * @ingroup developer
 */
#define error(msg) \
    std::cerr << "error: " << msg << std::endl; \
    exit(EXIT_FAILURE);

/**
 * Contents of initial `doxide.yaml` file.
 * 
 * @ingroup developer
 */
extern const char* init_doxide_yaml;

/**
 * Contents of initial `mkdocs.yaml` file.
 * 
 * @ingroup developer
 */
extern const char* init_mkdocs_yaml;

/**
 * Contents of initial `docs/javascripts/mathjax.js` file.
 * 
 * @ingroup developer
 */
extern const char* init_docs_javascripts_mathjax_js;

/**
 * Contents of initial `docs/stylesheets/doxide.css` file.
 * 
 * @ingroup developer
 */
extern const char* init_docs_stylesheets_doxide_css ;

/**
 * Contents of initial `docs/overrides/partials/copyright.html` file.
 * 
 * @ingroup developer
 */
extern const char* init_docs_overrides_partials_copyright_html;

/**
 * Query for entities in C++ sources.
 * 
 * @ingroup developer
 */
extern const char* query_cpp;

/**
 * Write a whole file.
 * 
 * @ingroup developer
 */
void write_file(const std::string& contents, const std::filesystem::path& dst);

/**
 * Write a whole file, prompting to confirm overwrite if it already exists.
 * 
 * @ingroup developer
 */
void write_file_prompt(const std::string& contents, const std::filesystem::path& dst);

/**
 * Copy a file, prompting to confirm overwrite if the destination already
 * exists.
 * 
 * @ingroup developer
 */
void copy_file_prompt(const std::filesystem::path& src, const std::filesystem::path& dst);

/**
 * Read in a whole file.
 * 
 * @ingroup developer
 */
std::string gulp(const std::filesystem::path& src);

/**
 * Tree-sitter C++ language handle.
 * 
 * @ingroup developer
 */
extern "C" const TSLanguage* tree_sitter_cpp();

/**
 * Tree-sitter CUDA language handle.
 * 
 * @ingroup developer
 */
extern "C" const TSLanguage* tree_sitter_cuda();
