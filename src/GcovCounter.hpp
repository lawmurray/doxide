#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * Processor for Gcov coverage files.
 * 
 * @ingroup developer
 */
class GcovCounter {
public:
  /**
   * Read in a Gcov JSON file and update execution counts.
   * 
   * @param root Root entity.
   */
  void count(Entity& root);
};
