#pragma once

#include "doxide.hpp"

/**
 * Parser for YAML config file.
 */
class Config {
public:
  using map_type = std::unordered_map<std::string,std::list<std::string>>;

  /**
   * Constructor.
   */
  Config();

  /**
   * Destructor.
   */
  ~Config();

  /**
   * Parse the file.
   * 
   * @return The contents of the file.
   */
  map_type parse();

private:
  void parseMapping();
  void parseSequence();
  void parseScalar();

  /**
   * LibYAML parser.
   */
  yaml_parser_t parser;

  /**
   * LibYAML event.
   */
  yaml_event_t event;

  /**
   * File.
   */
  FILE* file;

  /**
   * Stack of dictionary keys.
   */
  std::stack<std::string> keys;

  /**
   * Contents, populated during parse.
   */
  map_type contents;
};
