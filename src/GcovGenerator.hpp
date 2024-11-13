#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

#include <map>
#include <set>

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
   * @param node Root node.
   */
  void generate(const Entity& node);

private:
  /**
   * Collate coverage information for a single entity.
   */
  void collate(const Entity& node);

  /**
   * Line coverage information. Keyed by source file name, value is set of
   * covered lines.
   */
  std::map<std::string,std::set<int>> coverage;
};
