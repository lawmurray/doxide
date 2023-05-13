#pragma once

#include "doxide.hpp"
#include "doxide/Tokenizer.hpp"
#include "doxide/Node.hpp"

class Parser {
public:
  /**
   * Parse a source file.
   */
  void parse(const std::string& file);

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
   * With no arguments, the default value of `~ALL` for @p valid will simply
   * return the next token.
   */
  Token consume(const uint64_t valid = ~ALL, const bool delimiters = true);

  void parseGlobal();
  Node parseNamespace(const Token& first);
  Node parseClass(const Token& first);
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
