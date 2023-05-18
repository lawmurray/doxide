#pragma once

#include "doxide.hpp"

/**
 * Node types.
 */
enum class NodeType {
  NONE,
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
   * Constructor.
  */
  Node();

  /**
   * Add a child node.
   */
  void add(const Node& node);

  /**
   * Merge another namespace node.
   */
  void merge(const Node& node);

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
   * Alternative brief description.
   */
  std::string brief;

  /**
   * Node type.
   */
  NodeType type;

  /**
   * Hide this node?
   */
  bool hide;
};
