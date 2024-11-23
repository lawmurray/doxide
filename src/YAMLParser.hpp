#pragma once

#include "doxide.hpp"
#include "YAMLNode.hpp"

/**
 * Parser for YAML config files and YAML frontmatter of Markdown files. Also
 * works for JSON, as YAML is nowadays a superset of JSON.
 * 
 * @ingroup developer
 */
class YAMLParser {
public:
  /**
   * Constructor.
   *
   * @param filename File name.
   */
  YAMLParser();

  /**
   * Destructor.
   */
  ~YAMLParser();

  /**
   * Parse the file.
   * 
   * @return The contents of the file.
   */
  YAMLNode parse(const std::filesystem::path& filename);

private:
  void parseMapping(const std::filesystem::path& filename, YAMLNode& node);
  void parseSequence(const std::filesystem::path& filename, YAMLNode& node);
  void parseValue(const std::filesystem::path& filename, YAMLNode& node);

  /**
   * LibYAML parser.
   */
  yaml_parser_t parser;

  /**
   * LibYAML event.
   */
  yaml_event_t event;
};
