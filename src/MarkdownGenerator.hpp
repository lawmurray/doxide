#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * Markdown generator.
 * 
 * @ingroup developer
 */
class MarkdownGenerator {
public:
  /**
   * Constructor.
   * 
   * @param output Output directory.
   */
  MarkdownGenerator(const std::filesystem::path& output);

  /**
   * Generate documentation.
   * 
   * @param root Root entity.
   */
  void generate(const Entity& root);

  /**
   * Generate coverage.
   * 
   * @param root Root entity.
   */
  void coverage(const Entity& root);

  /**
   * Clean up after generation, removing files from old runs. Traverses the
   * output directory, removing any Markdown files with 'generator: doxide' in
   * their YAML frontmatter that were not generated by previous calls of
   * `generate()`.
   */
  void clean();

private:
  /**
   * Recursively generate documentation.
   * 
   * @param output Output directory.
   * @param entity Entity for which to generate documentation.
   */
  void generate(const std::filesystem::path& output, const Entity& entity);

  /**
   * Recursively generate coverage.
   * 
   * @param output Output directory.
   * @param entity Entity for which to generate coverage.
   */
  void coverage(const std::filesystem::path& output, const Entity& entity);

  /**
   * Recursively generate coverage table data.
   * 
   * @param entity Entity for which to generate coverage.
   * @param root Root entity for the current page. This is used to determine
   * which are rows should be visible initially.
   * @param out Output stream.
   */
  static void coverage_data(const Entity& entity, const Entity& root,
      std::ofstream& out);

  /**
   * Recursively generate coverage table footer.
   * 
   * @param entity Entity for which to generate coverage.
   * @param root Root entity for the current page. This is used to determine
   * which are rows should be visible initially.
   * @param out Output stream.
   */
  static void coverage_foot(const Entity& entity, const Entity& root,
      std::ofstream& out);

  /**
   * Produce sunburst chart of code coverage for entity.
   * 
   * @param entity Entity for which to generate sunburst.
   * @param root Root entity for the current page. This is used to determine
   * paths relative to the root.
   * @param out Output stream.
   */
  static void sunburst(const Entity& entity, const Entity& root,
      std::ofstream& out);

  /**
   * Produce data for sunburst chart of code coverage for entity.
   * 
   * @param entity Entity for which to generate sunburst.
   * @param root Root entity for the current page. This is used to determine
   * paths relative to the root.
   * @param out Output stream.
   */
  static void sunburst_data(const Entity& entity, const Entity& root,
      std::ofstream& out);

  /**
   * Produce a relative path.
   */
  static std::string relative(const std::filesystem::path& path,
      const std::filesystem::path& base);

  /**
   * Can the file be written? To be overwritten, the file must either not
   * exist, or exists but has 'generator: doxide' in its YAML frontmatter.
   */
  static bool can_write(const std::filesystem::path& path);

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
   * Convert coverage percentage to hexadecimal color.
   */
  static std::string color(const double percent);

  /**
   * Convert coverage percentage to icon.
   */
  static std::string icon(const double percent);

  /**
   * Convert a list of entities to a list of pointers to entities, optionally
   * sorting by name.
   * 
   * @param entities List of entities.
   * @param sort Sort by name?
   */
  static std::list<const Entity*> view(const std::list<Entity>& entities,
      const bool sort);

  /**
   * Output directory.
   */
  std::filesystem::path output;

  /**
   * Set of files generated during the last call to generate().
   */
  std::unordered_set<std::filesystem::path> files;
};
