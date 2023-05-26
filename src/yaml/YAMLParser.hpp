#pragma once

#include "doxide.hpp"
#include "yaml/YAMLNode.hpp"

/**
 * Parser for YAML config files and YAML frontmatter of Markdown files.
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
   * Parse the file.
   * 
   * @return The contents of the file.
   */
  YAMLNode parse(const std::string_view& contents);

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
