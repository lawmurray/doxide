#pragma once

#include "doxide.hpp"

/**
 * Entity types.
 */
enum class EntityType {
  NONE,
  NAMESPACE,
  GROUP,
  TYPE,
  VARIABLE,
  FUNCTION,
  OPERATOR,
  ENUMERATOR,
  MACRO,
  FILE
};

/**
 * Entity.
 */
struct Entity {
  using map_type = std::map<std::string,Entity>;
  using multimap_type = std::multimap<std::string,Entity>;

  /**
   * Constructor.
  */
  Entity();

  /**
   * Add child entity.
   * 
   * @param o Child entity.
   * 
   * If the child has `ingroup` set, then will search for and add to that
   * group instead.
   */
  void add(const Entity& o);

  /**
   * Merge the children of another entity into this one.
   * 
   * @param o Other entity.
   */
  void merge(const Entity& o);

  /**
   * Add child entity to a group.
   * 
   * @param o Child entity with `ingroup` set.
   * 
   * @return True if a group of the given name was found, in which case @p o
   * will have been added to it, false otherwise.
   */
  bool addToGroup(const Entity& o);

  /**
   * Add child entity.
   * 
   * @param o Child entity.
   * 
   * If the child has `ingroup` set, it is ignored.
   */
  void addToThis(const Entity& o);

  /**
   * Child namespaces.
   */
  map_type namespaces;

  /**
   * Child groups.
   */
  map_type groups;

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
   * Child enumerators.
   */
  map_type enumerators;

  /**
   * Child macros.
   */
  map_type macros;

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
   * Entity title. This is used for the title of the page.
   */
  std::string title;

  /**
   * Alternative brief description.
   */
  std::string brief;

  /**
   * Group to which this belongs.
   */
  std::string ingroup;

  /**
   * Entity type.
   */
  EntityType type;

  /**
   * Hide this node?
   */
  bool hide;
};
