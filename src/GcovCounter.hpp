#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * Gcov processor for coverage data.
 * 
 * @ingroup developer
 */
class GcovCounter {
public:
  /**
   * Read in file and update coverage data.
   * 
   * @param file Coverage file.
   * @param root Root entity.
   */
  void count(const std::filesystem::path& file, Entity& root);
};
