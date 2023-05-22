#pragma once

#include "doxide.hpp"
#include "Entity.hpp"
#include "doc/DocTranslator.hpp"

/**
 * C++ source parser.
 */
class CppParser {
public:
  /**
   * Constructor.
   */
  CppParser();

  /**
   * Destructor.
   */
  ~CppParser();

  /**
   * Parse C++ source code.
   * 
   * @param source C++ source code.
   */
  void parse(const std::string_view& source);

  /**
   * Get the root node.
   */
  const Entity& root() const {
    return global;
  }

private:
  /**
   * Parse the next entity.
   */
  void parseEntity(const std::string_view& source, TSTreeCursor& cursor,
      Entity& parent);

  /**
   * Tree-sitter parser.
   */
  TSParser* parser;

  /**
   * Global namespace.
   */
  Entity global;

  /**
   * Translator for comments.
   */
  DocTranslator translator;
};
