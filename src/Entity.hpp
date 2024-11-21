#pragma once

#include "doxide.hpp"

#include <filesystem>

/**
 * Entity types.
 * 
 * @ingroup developer
 */
enum class EntityType {
  ROOT,
  NAMESPACE,
  TEMPLATE,
  GROUP,
  TYPE,
  CONCEPT,
  VARIABLE,
  FUNCTION,
  OPERATOR,
  ENUMERATOR,
  MACRO,
  DIR,
  FILE
};

/**
 * Entity in a C++ source file, e.g. variable, function, class, etc.
 * 
 * @ingroup developer
 */
struct Entity {
  /**
   * Child entities are stored in a list, rather than map by name, to preserve
   * declaration order. They may be sorted by name on output.
   */
  using list_type = std::list<Entity>;

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
  void add(Entity&& o);

  /**
   * Merge the children of another entity into this one.
   * 
   * @param o Other entity.
   */
  void merge(Entity&& o);

  /**
   * Does a file exist of the given name?
   * 
   * @param path File path.
   */
  bool exists(std::filesystem::path& path) const;

  /**
   * Get a file of the given name. The file must exist (use `exists()`).
   *
   * @param path File path.
   *
   * @return List of entities giving the full path to the file. The last
   * entity represents the file itself, the preceding entities its
   * subdirectories.
   */
  std::list<Entity*> get(std::filesystem::path& path);

  /**
   * Clear the entity.
   */
  void clear();

  /**
   * Child namespaces.
   */
  list_type namespaces;

  /**
   * Child groups.
   */
  list_type groups;

  /**
   * Child types.
   */
  list_type types;

  /**
   * Child concepts.
   */
  list_type concepts;

  /**
   * Child variables.
   */
  list_type variables;

  /**
   * Child functions.
   */
  list_type functions;

  /**
   * Child operators.
   */
  list_type operators;

  /**
   * Child enumerators.
   */
  list_type enums;

  /**
   * Child macros.
   */
  list_type macros;

  /**
   * Child directories.
   */
  list_type dirs;

  /**
   * Child files.
   */
  list_type files;

  /**
   * Entity name (e.g. name of variable, function, class). For a file or
   * directory this is the full path.
   */
  std::string name;

  /**
   * Entity declaration (e.g. function signature). For a file this is its full
   * contents.
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
   * Path of source file.
   */
  std::filesystem::path path;

  /**
   * Starting line in the source file.
   */
  uint32_t start_line;

  /**
   * Ending line in the source file.
   */
  uint32_t end_line;

  /**
   * For a file only, execution counts for lines. -1 for a line indicates that
   * it is excluded.
   */
  std::vector<int> line_counts;

  /**
   * Number of lines included in coverage counts.
   */
  int lines_included;

  /**
   * Number of lines covered in coverage counts.
   */
  int lines_covered;

  /**
   * Current indent level of the documentation comment for this entity.
   */
  int indent;

  /**
   * Entity type.
   */
  EntityType type;

  /**
   * Hide this node?
   */
  bool hide;

  /**
   * Does this node have one or more visible children?
   */
  bool visibleChildren;

private:
  /**
   * Add child entity to a group.
   * 
   * @param o Child entity with `ingroup` set.
   * 
   * @return True if a group of the given name was found, in which case @p o
   * will have been added to it, false otherwise.
   */
  bool addToGroup(Entity&& o);

  /**
   * Add child entity.
   * 
   * @param o Child entity.
   * 
   * If the child has `ingroup` set, it is ignored.
   */
  void addToThis(Entity&& o);
};
