#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

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
  const Entity& root() const;

private:
  void parseEntity(const char* src, TSTreeCursor& cursor, Entity& parent);
  void interpret(const std::string_view& comment, Entity& o);

  /**
   * Global namespace.
   */
  Entity global;
};
