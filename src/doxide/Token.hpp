#pragma once

#include "doxide.hpp"

/**
 * Token types. Closing delimiters must be one shift left of their opening
 * counterparts.
 */
enum TokenType : uint64_t {
  NONE = 0ull,
  BRACE = 1ull << 1,
  BRACE_CLOSE = 1ull << 2,
  BRACKET = 1ull << 3,
  BRACKET_CLOSE = 1ull << 4,
  PAREN = 1ull << 5,
  PAREN_CLOSE = 1ull << 6,
  ANGLE = 1ull << 7,
  ANGLE_CLOSE = 1ull << 8,
  DOC = 1ull << 9,
  DOC_CLOSE = 1ull << 10,
  DOC_COMMAND = 1ull << 11,
  DOC_ESCAPE = 1ull << 12,
  DOC_PARA = 1ull << 13,
  DOC_LINE = 1ull << 14,
  COMMENT = 1ull << 15,
  EOL_COMMENT = 1ull << 16,
  STRING = 1ull << 17,
  NAMESPACE = 1ull << 18,
  TYPE = 1ull << 19,
  OPERATOR = 1ull << 20,
  COLON = 1ull << 21,
  SEMICOLON = 1ull << 22,
  SENTENCE = 1ull << 23,
  EQUALS = 1ull << 24,
  TILDE = 1ull << 25,
  PUNCT = 1ull << 26,
  WHITESPACE = 1ull << 27,
  WORD = 1ull << 28,
  ANY = ~0ull
};

/**
 * Token patterns. Order is important, as match to an earlier pattern
 * precludes a match to a later.
 */
static auto regexes = {
  std::make_pair(BRACE, std::regex("\\{")),
  std::make_pair(BRACE_CLOSE, std::regex("\\}")),
  std::make_pair(BRACKET, std::regex("\\[")),
  std::make_pair(BRACKET_CLOSE, std::regex("\\]")),
  std::make_pair(ANGLE, std::regex("<")),
  std::make_pair(ANGLE_CLOSE, std::regex(">")),
  std::make_pair(PAREN, std::regex("\\(")),
  std::make_pair(PAREN_CLOSE, std::regex("\\)")),
  std::make_pair(DOC, std::regex("/\\*\\*")),
  std::make_pair(DOC_CLOSE, std::regex("\\*/")),
  std::make_pair(DOC_COMMAND, std::regex("[@\\\\](?:param(?:\\[(?:in|out|in,out)\\])?|\\w+|f[\\$\\[\\]])")),
  std::make_pair(DOC_ESCAPE, std::regex("[\\\\%]\\S")),
  std::make_pair(DOC_PARA, std::regex("(?:(?:^|\\n)[ \t]*\\*(?!/)[ \t]*){2}")),
  std::make_pair(DOC_LINE, std::regex("(?:^|\\n)[ \t]*\\*(?!/)[ \t]*")),
  std::make_pair(COMMENT, std::regex("/\\*(?:\\n|.)*?\\*/")),
  std::make_pair(EOL_COMMENT, std::regex("//.*?$")),
  std::make_pair(STRING, std::regex("\"(?:\\\\\"|[^\"])*?\"")),
  std::make_pair(NAMESPACE, std::regex("\\bnamespace\\b")),
  std::make_pair(TYPE, std::regex("\\b(?:class|struct|enum|typedef)\\b")),
  std::make_pair(OPERATOR, std::regex("\\boperator(?:\\+|-|\\*|/|%|^|&|\\||~|!|=|<|>|\\+=|-=|\\*=|/=|%=|^=|&=|\\|=|<<|>>|>>=|<<=|==|!=|<=|>=|<=>|&&|\\|\\||\\+\\+|--|,|->\\*|->|\\(\\)|\\[\\])")),
  std::make_pair(COLON, std::regex(":")),
  std::make_pair(SEMICOLON, std::regex(";")),
  std::make_pair(SENTENCE, std::regex("[.!?]")),
  std::make_pair(EQUALS, std::regex("=")),
  std::make_pair(TILDE, std::regex("~")),
  std::make_pair(PUNCT, std::regex("[,&|\\-+*/%^@#'`]")),
  std::make_pair(WHITESPACE, std::regex("\\s+")),
  std::make_pair(WORD, std::regex("[^{}\\[\\]()<>'`\":;=,.?!~&|\\-+*/%^:@#\\n \\t\\v\\f\\r]+"))
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
      const std::string::const_iterator& first = {},
      const std::string::const_iterator& last = {}) :
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
  std::string::const_iterator first;

  /**
   * Iterator to one-past-last character.
   */
  std::string::const_iterator last;
};
