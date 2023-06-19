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
   * @param file C++ source file name.
   * @param global Global namespace.
   */
  void parse(const std::string& file, Entity& global);

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
