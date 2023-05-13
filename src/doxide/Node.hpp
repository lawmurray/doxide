#pragma once

#include "doxide.hpp"

/**
 * Token types.
 */
enum class NodeType {
  NAMESPACE,
  CLASS,
  STRUCT,
  TYPE,
  ENUM,
  VARIABLE,
  FUNCTION,
  OPERATOR
};

/**
 * Token.
 */
struct Node {
  using map_type = std::map<std::string,Node>;

  /**
   * Node type.
   */
  NodeType type;

  /**
   * Entity name (e.g. name of variable, function, class).
   */
  std::string name;

  /**
   * Entity declaration (e.g. function signature).
   */
  std::string decl;

  /**
   * Child classes.
   */
  map_type classes;

  /**
   * Child structs.
   */
  map_type structs;

  /**
   * Child types.
   */
  map_type types;

  /**
   * Child enums.
   */
  map_type enums;

  /**
   * Child variables.
   */
  map_type variables;

  /**
   * Child functions.
   */
  map_type functions;

  /**
   * Child operators.
   */
  map_type operators;
};
