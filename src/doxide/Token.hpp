#pragma once

#include "doxide.hpp"

/**
 * Token types.
 */
enum TokenType {
  DOC_COMMENT,
  INLINE_COMMENT,
  LINE_COMMENT,
  BRACE_OPEN,
  BRACE_CLOSE,
  BRACKET_OPEN,
  BRACKET_CLOSE,
  PAREN_OPEN,
  PAREN_CLOSE,
  ANGLE_OPEN,
  ANGLE_CLOSE,
  CHAR,
  STRING,
  COLON,
  SEMICOLON,
  NAMESPACE,
  CLASS,
  STRUCT,
  SPACE,
  OTHER,
  END_OF_LINE,
  END_OF_FILE
};

/**
 * Token.
 */
class Token {
public:
  /**
   * Constructor.
   */
  Token(const TokenType& type, const std::string::const_iterator& first,
      const std::string::const_iterator& last);

  /**
   * Get token as string.
   */
  std::string_view str() const;

  /**
   * Token type.
   */
  TokenType type;

  /**
   * Iterator to first character.
   */
  std::string::const_iterator first;

  /**
   * Iterator to one-past-last character.
   */
  std::string::const_iterator last;
};
