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
   * @param stop Bitmask giving the token type on which to stop and return.
   * 
   * @return Last token consumed.
   * 
   * Tokens are consumed until one is encountered with a type in @p stop,
   * which is then returned. If @p stop is `ANY` then the next token is
   * returned.
   */
  Token consume(const uint64_t stop = ANY);

  void parseGlobal();
  Node parseNamespace(const Token& first);
  Node parseDocs(const Token& first);

  /**
   * Tokenizer.
   */
  Tokenizer tokenizer;

  /**
   * Global namespace.
   */
  Node global;

  /**
   * Last word token read.
   */
  Token word;
};
