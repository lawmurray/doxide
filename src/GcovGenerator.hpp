#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

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
