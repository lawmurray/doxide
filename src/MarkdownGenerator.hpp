#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * Markdown generator.
 */
class MarkdownGenerator {
public:
  /**
   * Generate documentation.
   * 
   * @param dir Output directory.
   * @param node Root node.
   */
  void generate(const std::filesystem::path& dir, const Entity& node);

private:
  /**
   * Produce the YAML frontmatter for an entity.
   */
  static std::string frontmatter(const Entity& entity);

  /**
   * Produce title for an entity.
   */
  static std::string title(const Entity& entity);

  /**
   * Produce brief description for an entity.
   */
  static std::string brief(const Entity& entity);

  /**
   * Reduce to a single line.
   */
  static std::string line(const std::string& str);

  /**
   * Indent lines.
   */
  static std::string indent(const std::string& str);

  /**
   * Sanitize for a string, escaping double quotes and backslashes.
   */
  static std::string stringify(const std::string& str);

  /**
   * Sanitize for HTML, replacing special characters with entities. Also
   * replaces some characters that might trigger Markdown formatting.
   */
  static std::string htmlize(const std::string& str);

  /**
   * Sanitize for a file name or internal anchor.
   */
  static std::string sanitize(const std::string& str);

  /**
   * Convert a list of entities to a list of pointers to entities, optionally
   * sorting by name.
   * 
   * @param entities List of entities.
   * @param sort Sort by name?
   */
  static std::list<const Entity*> view(const std::list<Entity>& entities,
      const bool sort);
};
