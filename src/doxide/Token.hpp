#pragma once

#include "doxide.hpp"

/**
 * Token types.
 */
enum class TokenType {
  DOC_COMMENT_OPEN,
  DOC_COMMENT_CLOSE,
  INLINE_COMMENT_OPEN,
  INLINE_COMMENT_CLOSE,
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
  ENUM,
  USING,
  FRIEND,
  SPACE,
  OTHER,
  END_OF_LINE,
  END_OF_FILE
};

/**
 * Token.
 */
struct Token {
public:
  /**
   * Get token as string.
   */
  std::string_view str() const {
    return std::string_view(first, last);
  }

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
