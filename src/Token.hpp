#pragma once

#include "doxide.hpp"

/**
 * Token types. Closing delimiters must be one shift left of their opening
 * counterparts.
 */
enum TokenType : int {
  NONE = 0,
  OPEN_AFTER = 1 << 1,   // opening sequence for doc comment after entity
  OPEN_BEFORE = 1 << 2,  // opening sequence for doc comment before entity
  CLOSE = 1 << 3,        // closing sequence for doc comment
  COMMAND = 1 << 4,
  PARA = 1 << 5,
  LINE = 1 << 6,
  SENTENCE = 1 << 7,
  WHITESPACE = 1 << 8,
  WORD = 1 << 9,
  STAR = 1 << 10,
  SLASH = 1 << 11,
  ANY = ~0
};

/**
 * Token patterns. Order is important, as a match to an earlier pattern
 * precludes a match to a later.
 */
static auto regexes = {
  std::make_pair(OPEN_AFTER, std::regex("(?:/\\*\\*|/\\*!|///|//!)<[ \t]*")),
  std::make_pair(OPEN_BEFORE, std::regex("(?:/\\*\\*|/\\*!|///|//!)[ \t]*")),
  std::make_pair(CLOSE, std::regex("\\*/")),
  std::make_pair(COMMAND, std::regex("[@\\\\](?:param(?:\\[(?:in|out|in,out)\\])?|\\w+|@|/|f[\\$\\[\\]])")),

  /* the end of a paragraph is either two new lines or one new line with a
   * command to come */
  std::make_pair(PARA, std::regex("\\s*\\n[ \t]*(?:\\*(?!/))?[ \t]*(?:(?=@)|\\n[ \t]*(?:\\*(?!/))?[ \t]*)")),

  /* the end of a line is one new line, as long as there is not a command to
   * come (which would denote the end of a paragraph instead) */
  std::make_pair(LINE, std::regex("\\s*\\n[ \t]*(?:\\*(?!/))?[ \t]*(?!@)")),

  std::make_pair(SENTENCE, std::regex("[.!?]")),
  std::make_pair(WHITESPACE, std::regex("\\s+")),
  std::make_pair(WORD, std::regex("[^\\s\\*/]+")),
  std::make_pair(STAR, std::regex("\\*")),
  std::make_pair(SLASH, std::regex("/"))
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
  Token(const TokenType& type,
      const std::string_view::const_iterator& first,
      const std::string_view::const_iterator& last) :
      type(type),
      first(first),
      last(last) {
    //
  }

  /**
   * Get token as string.
   */
  std::string_view str() const {
    /* std::string_view(first, last) ought to work with C++20 support, but
     * using the below overload of the constructor extends support to some
     * older compilers, such as gcc with gnu++2a support only, and MSVC 2022 */
    return std::string_view(&*first, std::distance(first, last));
  }

  /**
   * Get substring of the token as a string.
   * 
   * @param pos Position of the first character.
   */
  std::string_view substr(size_t pos = 0) const {
    return str().substr(pos);
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
