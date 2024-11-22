#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * JSON generator for coverage data following gcov/gcovr schema.
 * 
 * @ingroup developer
 */
class JSONGenerator {
public:
  /**
   * Generate coverage data.
   * 
   * @param root Root entity.
   */
  void generate(const Entity& root);

private:
  /**
   * Generate documentation.
   * 
   * @param root Root entity.
   * @param nfiles Number of files output so far.
   * 
   * @return Updated number of files output so far.
   */
  int generate(const Entity& root, int nfiles);
};
