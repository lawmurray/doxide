#pragma once

#include "doxide.hpp"
#include "File.hpp"

/**
 * Gcov JSON generator.
 * 
 * @ingroup developer
 */
class GcovGenerator {
public:
  /**
   * Generate documentation.
   * 
   * @param files Source files.
   */
  void generate(const std::list<File>& parser);
};
