#pragma once

#include "doxide.hpp"

/**
 * DocToken types. Closing delimiters must be one shift left of their opening
 * counterparts.
 */
enum DocTokenType : int {
  NONE = 0,
  DOC = 1 << 1,
  DOC_CLOSE = 1 << 2,
  DOC_COMMAND = 1 << 3,
  DOC_ESCAPE = 1 << 4,
  DOC_PARA = 1 << 5,
  DOC_LINE = 1 << 6,
  SENTENCE = 1 << 7,
  WORD = 1 << 8,
  WHITESPACE = 1 << 9,
  ANY = ~0
};

/**
 * DocToken patterns. Order is important, as match to an earlier pattern
 * precludes a match to a later.
 */
static auto regexes = {
  std::make_pair(DOC, std::regex("/\\*\\*")),
  std::make_pair(DOC_CLOSE, std::regex("\\*/")),
  std::make_pair(DOC_COMMAND, std::regex("[@\\\\](?:param(?:\\[(?:in|out|in,out)\\])?|\\w+|f[\\$\\[\\]])")),
  std::make_pair(DOC_ESCAPE, std::regex("[\\\\%]\\S")),
  std::make_pair(DOC_PARA, std::regex("\\s*\\n[ \t]*(?:\\*(?!/))?[ \t]*\\n[ \t]*(?:\\*(?!/))?[ \t]*")),
  std::make_pair(DOC_LINE, std::regex("\\s*\\n[ \t]*(?:\\*(?!/))?[ \t]*")),
  std::make_pair(SENTENCE, std::regex("[.!?]")),
  std::make_pair(WORD, std::regex("\\S+")),
  std::make_pair(WHITESPACE, std::regex("\\s+"))
};

/**
 * DocToken.
 * 
 * A token is only valid for the lifetime of the DocTokenizer that produced it,
 * as it contains a reference to a substring of the source file.
 */
class DocToken {
public:
  /**
   * Constructor.
   */
  DocToken(const DocTokenType& type = NONE,
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
   * DocToken type.
   */
  DocTokenType type;

  /**
   * Iterator to first character.
   */
  std::string_view::const_iterator first;

  /**
   * Iterator to one-past-last character.
   */
  std::string_view::const_iterator last;
};
