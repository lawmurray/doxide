#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * C++ source parser.
 */
class Parser {
public:
  /**
   * Constructor.
   */
  Parser();

  /**
   * Destructor.
   */
  ~Parser();

  /**
   * Parse C++ source code.
   * 
   * @param source C++ source code.
   * @param global Global namespace.
   * 
   * @return Was parsing successful?
   */
  bool parse(const std::string_view& source, Entity& global);

  /**
   * Translate documentation comment.
   * 
   * @param comment Documentation comment.
   * @param entity Entity to document.
   */
  void translate(const std::string_view& comment, Entity& entity);


private:
  /**
   * Tree-sitter parser.
   */
  TSParser* parser;

  /**
   * Tree-sitter query.
   */
  TSQuery* query;
};
