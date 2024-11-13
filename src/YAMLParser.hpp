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
   */
  YAMLParser();

  /**
   * Destructor.
   */
  ~YAMLParser();

  /**
   * Parse a file.
   * 
   * @param file File name.
   * 
   * @return The contents of the file.
   */
  YAMLNode parse(const std::string& file);

private:
  void parseMapping(YAMLNode& node);
  void parseSequence(YAMLNode& node);
  void parseValue(YAMLNode& node);

  /**
   * LibYAML parser.
   */
  yaml_parser_t parser;

  /**
   * LibYAML event.
   */
  yaml_event_t event;
};
