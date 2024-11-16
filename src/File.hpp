#pragma once

#include "doxide.hpp"

#include <set>

/**
 * Source file with line data.
 * 
 * @ingroup developer
 */
struct File {
  /**
   * File name.
   */
  std::string filename;

  /**
   * Complete post-processed source code.
   */
  std::string content;

  /**
   * Numbers of executable lines, zero-based.
   */
  std::set<int> lines;
};
