#pragma once

#include "doxide.hpp"

/**
 * Node types.
 */
enum class NodeType {
  NAMESPACE,
  TYPE,
  VARIABLE,
  FUNCTION,
  OPERATOR
};

/**
 * Node.
 */
struct Node {
  using map_type = std::map<std::string,Node>;
  using multimap_type = std::multimap<std::string,Node>;

  /**
   * Add a child node.
   */
  void add(const Node& node);

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
  multimap_type functions;

  /**
   * Child operators.
   */
  multimap_type operators;
};
