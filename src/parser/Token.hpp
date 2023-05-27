#pragma once

#include "doxide.hpp"

/**
 * Token types. Closing delimiters must be one shift left of their opening
 * counterparts.
 */
enum TokenType : int {
  NONE = 0,
  OPEN = 1 << 1,
  CLOSE = 1 << 2,
  STRING = 1 << 3,
  INLINE_MATH = 1 << 4,
  DISPLAY_MATH = 1 << 5,
  INLINE_CODE = 1 << 6,
  DISPLAY_CODE = 1 << 7,
  COMMAND = 1 << 8,
  ESCAPE = 1 << 9,
  PARA = 1 << 10,
  LINE = 1 << 11,
  SENTENCE = 1 << 12,
  WORD = 1 << 13,
  WHITESPACE = 1 << 14,
  ANY = ~0
};

/**
 * Token patterns. Order is important, as match to an earlier pattern
 * precludes a match to a later.
 */
static auto regexes = {
  std::make_pair(OPEN, std::regex("/\\*\\*")),
  std::make_pair(CLOSE, std::regex("\\*/")),
  std::make_pair(STRING, std::regex("\"(?:\\\\\"|[^\"])*\"")),
  std::make_pair(INLINE_MATH, std::regex("\\$(?:\\\\\\$|[^\\$])*\\$")),
  std::make_pair(DISPLAY_MATH, std::regex("\\$\\$(?:\\\\\\$|[^\\$])*\\$\\$")),
  std::make_pair(INLINE_CODE, std::regex("`(?:\\\\`|[^`])*`")),
  std::make_pair(DISPLAY_CODE, std::regex("```(?:\\\\`|[^`])*```")),
  std::make_pair(COMMAND, std::regex("[@\\\\](?:param(?:\\[(?:in|out|in,out)\\])?|\\w+|f[\\$\\[\\]])")),
  std::make_pair(ESCAPE, std::regex("[\\\\%]\\S")),
  std::make_pair(PARA, std::regex("\\s*\\n[ \t]*(?:\\*(?!/))?[ \t]*\\n[ \t]*(?:\\*(?!/))?[ \t]*")),
  std::make_pair(LINE, std::regex("\\s*\\n[ \t]*(?:\\*(?!/))?[ \t]*")),
  std::make_pair(SENTENCE, std::regex("[.!?]")),
  std::make_pair(WORD, std::regex("\\S+")),
  std::make_pair(WHITESPACE, std::regex("\\s+"))
};

/**
 * Token.
 * 
 * A token is only valid for the lifetime of the Tokenizer that produced it,
 * as it contains a reference to a substring of the source file.
 */
class Token {
public:
  /**
   * Constructor.
   */
  Token(const TokenType& type = NONE,
      const std::string_view::const_iterator& first = {},
      const std::string_view::const_iterator& last = {}) :
      type(type),
      first(first),
      last(last) {
    //
  }

  /**
   * Get token as string.
   */
  std::string_view str() const {
    return std::string_view(first, last);
  }

  /**
   * Get substring of the token as a string.
   * 
   * @param pos Position of the first character.
   */
  std::string_view substr(size_t pos = 0) const {
    return std::string_view(first, last).substr(pos);
  }

  /**
   * Token type.
   */
  TokenType type;

  /**
   * Iterator to first character.
   */
  std::string_view::const_iterator first;

  /**
   * Iterator to one-past-last character.
   */
  std::string_view::const_iterator last;
};
