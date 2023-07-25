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
 */
#define warn(msg) \
    std::cerr << "warning: " << msg << std::endl;

/**
 * Print an error message and exit.
 */
#define error(msg) \
    std::cerr << "error: " << msg << std::endl; \
    exit(EXIT_FAILURE);

/**
 * Contents of initial doxide.yaml
 */
extern const char* init_doxide_yaml;

/**
 * Contents of initial mkdocs.yaml
 */
extern const char* init_mkdocs_yaml;

/**
 * Contents of initial docs/javascripts/mathjax.js
 */
extern const char* init_docs_javascripts_mathjax_js;

/**
 * Contents of initial docs/stylesheets/doxide.css
 */
extern const char* init_docs_stylesheets_doxide_css ;

/**
 * Contents of initial docs/overrides/partials/copyright.html
 */
extern const char* init_docs_overrides_partials_copyright_html;

/**
 * Write a whole file.
 */
void write_file(const std::string& contents, const std::filesystem::path& dst);

/**
 * Write a whole file, prompting to confirm overwrite if it already exists.
 */
void write_file_prompt(const std::string& contents, const std::filesystem::path& dst);

/**
 * Copy a file, prompting to confirm overwrite if the destination already
 * exists.
 */
void copy_file_prompt(const std::filesystem::path& src, const std::filesystem::path& dst);

/**
 * Read in a whole file.
 */
std::string gulp(const std::filesystem::path& src);
