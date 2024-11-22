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
#include <regex>

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
 * Regular expression configuration.
 * 
 * @ingroup developer
 */
static const std::regex_constants::syntax_option_type regex_flags =
    std::regex_constants::ECMAScript|std::regex_constants::optimize;

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
