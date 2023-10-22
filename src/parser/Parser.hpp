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
   * @param after Does the documentation comment appear after the entity?
   */
  void translate(const std::string_view& comment, Entity& entity,
      const bool after = false);

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
