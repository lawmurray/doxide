#pragma once

#include "doxide.hpp"
#include "doxide/Node.hpp"

/**
 * Parser.
 */
class Parser {
public:
  /**
   * Parse a source file.
   */
  void parse(const std::string& file);

  /**
   * Get the root node.
   */
  const Node& root() const;

private:
  void parseNode(const char* src, TSTreeCursor& cursor, Node& parent);
  void interpret(const std::string_view& comment, Node& o);

  /**
   * Global namespace.
   */
  Node global;
};
