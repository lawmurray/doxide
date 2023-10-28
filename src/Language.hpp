#pragma once

#include "doxide.hpp"

extern "C" const TSLanguage* tree_sitter_cpp();

/**
 * Query for C++.
 */
extern const char* query_cpp;

/**
 * Query for preprocessor.
 */
extern const char* query_preprocess;
