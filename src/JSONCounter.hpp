#pragma once

#include "Entity.hpp"

/**
 * JSON processor for coverage data following gcov/gcovr schema.
 *
 * @ingroup developer
 */
class JSONCounter {
public:
  /**
   * Read in file and update coverage data.
   *
   * @param file Coverage file.
   * @param root Root entity.
   */
  void count(const std::filesystem::path& file, Entity& root);
};
