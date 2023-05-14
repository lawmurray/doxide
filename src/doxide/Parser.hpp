#pragma once

#include "doxide.hpp"
#include "doxide/Tokenizer.hpp"
#include "doxide/Node.hpp"

/**
 * Parser.
 */
class Parser {
public:
  /**
   * Parse a source file.
   */
  void parse(const std::string& file);

  /**
   * Get the root node.
   */
  const Node& root() const;

private:
  /**
   * Consume tokens until stopping criterion.
   * 
   * @param valid Bitmask giving the valid token types.
   * @param delimiters Match delimiters?
   * 
   * @return Last token consumed.
   * 
   * Tokens are consumed until one is encountered with a type not in @p valid,
   * which is returned. If @p delimeters is true, then delimiters must also
   * match.
   * 
   * The token type `NONE` is never valid. Given as the value of @p valid (the
   * default) means that the next token is returned, regardless of type.
   */
  Token consume(const uint64_t valid = NONE, const bool delimiters = true);

  void parseGlobal();
  Node parseNamespace(const Token& first);
  Node parseType(const Token& first);
  Node parseDocs(const Token& first);

  /**
   * Tokenizer.
   */
  Tokenizer tokenizer;

  /**
   * Global namespace.
   */
  Node global;
};
