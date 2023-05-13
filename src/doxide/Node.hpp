#pragma once

#include "doxide.hpp"

/**
 * Token types.
 */
enum class NodeType {
  NAMESPACE,
  TYPE,
  VARIABLE,
  FUNCTION,
  OPERATOR
};

/**
 * Token.
 */
struct Node {
  using map_type = std::map<std::string,Node>;

  void add(const Node& child) {

  }

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
   * Entity documentation.
   */
  std::string docs;

  /**
   * Child namespaces.
   */
  map_type namespaces;

  /**
   * Child types.
   */
  map_type types;

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
