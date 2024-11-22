#pragma once

#include "doxide.hpp"
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
   * @param root Root entity.
   */
  void count(Entity& root);
};
