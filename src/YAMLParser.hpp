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
   * @param max_docs Maximum number of documents to handle. Zero for no limit.
   * Set to 1 (the default) to read YAML frontmatter (delimited with `---` at
   * start and end) from a Markdown file.
   */
  YAMLParser(const std::string& filename, const int max_docs = 1);

  /**
   * Destructor.
   */
  ~YAMLParser();

  /**
   * Parse the file.
   * 
   * @return The contents of the file.
   */
  YAMLNode parse();

private:
  void parseMapping(YAMLNode& node);
  void parseSequence(YAMLNode& node);
  void parseValue(YAMLNode& node);

  /**
   * File name.
   */
  std::string filename;

  /**
   * Maximum number of documents to handle. Zero for no limit.
   */
  int max_docs;

  /**
   * LibYAML parser.
   */
  yaml_parser_t parser;

  /**
   * LibYAML event.
   */
  yaml_event_t event;
};
