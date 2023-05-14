#pragma once

#include "doxide.hpp"

/**
 * Token types.
 * 
 * Closing delimiters must be one shift left of their opening counterparts.
 */
enum TokenType : uint64_t {
  NONE = 0ull,
  DOC = 1ull << 0,
  COMMENT = 1ull << 1,
  EOL_COMMENT = 1ull << 2,
  BRACE = 1ull << 3,
  BRACE_CLOSE = 1ull << 4,
  BRACKET = 1ull << 5,
  BRACKET_CLOSE = 1ull << 6,
  PAREN = 1ull << 7,
  PAREN_CLOSE = 1ull << 8,
  ANGLE = 1ull << 9,
  ANGLE_CLOSE = 1ull << 10,
  CHAR = 1ull << 11,
  STRING = 1ull << 12,
  NAMESPACE = 1ull << 13,
  USING_NAMESPACE = 1ull << 14,
  TYPE = 1ull << 15,
  FRIEND_TYPE = 1ull << 16,
  OPERATOR = 1ull << 17,
  COLON = 1ull << 18,
  SEMICOLON = 1ull << 19,
  EQUALS = 1ull << 20,
  TILDE = 1ull << 21,
  PUNCT = 1ull << 22,
  WHITESPACE = 1ull << 23,
  WORD = 1ull << 24,
  ANY = ~0ull
};

/**
 * Token patterns.
 */
static auto regexes = {
  std::make_pair(TokenType::DOC, std::regex("/\\*\\*(?:\\n|.)*?\\*/")),
  std::make_pair(TokenType::COMMENT, std::regex("/\\*(?:\\n|.)*?\\*/")),
  std::make_pair(TokenType::EOL_COMMENT, std::regex("//.*?$")),
  std::make_pair(TokenType::BRACE, std::regex("\\{")),
  std::make_pair(TokenType::BRACE_CLOSE, std::regex("\\}")),
  std::make_pair(TokenType::BRACKET, std::regex("\\[")),
  std::make_pair(TokenType::BRACKET_CLOSE, std::regex("\\]")),
  std::make_pair(TokenType::ANGLE, std::regex("<")),
  std::make_pair(TokenType::ANGLE_CLOSE, std::regex(">")),
  std::make_pair(TokenType::PAREN, std::regex("\\(")),
  std::make_pair(TokenType::PAREN_CLOSE, std::regex("\\)")),
  std::make_pair(TokenType::CHAR, std::regex("'(?:\\\\'|[^'])*?'")),
  std::make_pair(TokenType::STRING, std::regex("\"(?:\\\\\"|[^\"])*?\"")),
  std::make_pair(TokenType::NAMESPACE, std::regex("\\bnamespace\\b")),
  std::make_pair(TokenType::USING_NAMESPACE, std::regex("\\busing\\s+namespace\\b")),
  std::make_pair(TokenType::TYPE, std::regex("\\b(?:class|struct|enum|typedef)\\b")),
  std::make_pair(TokenType::FRIEND_TYPE, std::regex("\\bfriend\\s+(?:class|struct)\\b")),
  std::make_pair(TokenType::OPERATOR, std::regex("\\boperator\\b")),
  std::make_pair(TokenType::COLON, std::regex(":")),
  std::make_pair(TokenType::SEMICOLON, std::regex(";")),
  std::make_pair(TokenType::EQUALS, std::regex("=")),
  std::make_pair(TokenType::TILDE, std::regex("~")),
  std::make_pair(TokenType::PUNCT, std::regex("[,.?!&|\\-+*/%^@#]")),
  std::make_pair(TokenType::WHITESPACE, std::regex("\\s+")),
  std::make_pair(TokenType::WORD, std::regex("[^{}\\[\\]()<>'\":;=,.?!~&|\\-+*/%^:@#\\n \\t\\v\\f\\r]+"))
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
