#pragma once

#include <string>
#include <regex>
#include <stack>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

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
