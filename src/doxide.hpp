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

#include <getopt.h>
#include <glob.h>
#include <yaml.h>
#include <tree_sitter/api.h>

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
