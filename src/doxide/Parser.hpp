#pragma once

#include "doxide.hpp"
#include "doxide/Node.hpp"

class Parser {
public:
  /**
   * Parse a source file.
   */
  void parse(const std::string& file);

private:
  /**
   * Global namespace.
   */
  Node global;
};
