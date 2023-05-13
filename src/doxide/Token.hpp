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
  DOC_CLOSE = 1ull << 1,
  COMMENT = 1ull << 2,
  COMMENT_CLOSE = 1ull << 3,
  EOL_COMMENT = 1ull << 4,
  BRACE = 1ull << 5,
  BRACE_CLOSE = 1ull << 6,
  BRACKET = 1ull << 7,
  BRACKET_CLOSE = 1ull << 8,
  PAREN = 1ull << 9,
  PAREN_CLOSE = 1ull << 10,
  ANGLE = 1ull << 11,
  ANGLE_CLOSE = 1ull << 12,
  CHAR = 1ull << 13,
  STRING = 1ull << 14,
  NAMESPACE = 1ull << 15,
  USING_NAMESPACE = 1ull << 16,
  CLASS = 1ull << 17,
  ENUM_CLASS = 1ull << 18,
  FRIEND_CLASS = 1ull << 19,
  STRUCT = 1ull << 20,
  ENUM_STRUCT = 1ull << 21,
  FRIEND_STRUCT = 1ull << 22,
  TYPEDEF = 1ull << 23,
  OPERATOR = 1ull << 24,
  COLON = 1ull << 25,
  SEMICOLON = 1ull << 26,
  EQUALS = 1ull << 27,
  PUNCT = 1ull << 28,
  SPACE = 1ull << 29,
  EOL = 1ull << 30,
  WORD = 1ull << 31,

  /* aggregate tokens */
  WHITESPACE = SPACE|EOL,
  DELIMITER = DOC|COMMENT|BRACE|BRACKET|PAREN|ANGLE
};

/**
 * Token patterns.
 */
static auto regexes = {
  std::make_pair(TokenType::DOC, std::regex("/\\*\\*")),
  std::make_pair(TokenType::DOC_CLOSE, std::regex("\\*/")),
  std::make_pair(TokenType::COMMENT, std::regex("/\\*")),
  std::make_pair(TokenType::COMMENT_CLOSE, std::regex("\\*/")),
  std::make_pair(TokenType::EOL_COMMENT, std::regex("//.*?$")),
  std::make_pair(TokenType::BRACE, std::regex("\\{")),
  std::make_pair(TokenType::BRACE_CLOSE, std::regex("\\}")),
  std::make_pair(TokenType::BRACKET, std::regex("\\[")),
  std::make_pair(TokenType::BRACKET_CLOSE, std::regex("\\]")),
  std::make_pair(TokenType::PAREN, std::regex("\\(")),
  std::make_pair(TokenType::PAREN_CLOSE, std::regex("\\)")),
  std::make_pair(TokenType::ANGLE, std::regex("<")),
  std::make_pair(TokenType::ANGLE_CLOSE, std::regex(">")),
  std::make_pair(TokenType::CHAR, std::regex("'(?:\\\\'|[^'])*?'")),
  std::make_pair(TokenType::STRING, std::regex("\"(?:\\\\\"|[^\"])*?\"")),
  std::make_pair(TokenType::NAMESPACE, std::regex("\\bnamespace\\b")),
  std::make_pair(TokenType::USING_NAMESPACE, std::regex("\\busing\\s+namespace\\b")),
  std::make_pair(TokenType::CLASS, std::regex("\\bclass\\b")),
  std::make_pair(TokenType::ENUM_CLASS, std::regex("\\benum\\s+class\\b")),
  std::make_pair(TokenType::FRIEND_CLASS, std::regex("\\bfriend\\s+class\\b")),
  std::make_pair(TokenType::STRUCT, std::regex("\\bstruct\\b")),
  std::make_pair(TokenType::ENUM_STRUCT, std::regex("\\benum\\s+struct\\b")),
  std::make_pair(TokenType::FRIEND_STRUCT, std::regex("\\bfriend\\s+struct\\b")),
  std::make_pair(TokenType::TYPEDEF, std::regex("\\btypedef\\b")),
  std::make_pair(TokenType::OPERATOR, std::regex("\\boperator\\b")),
  std::make_pair(TokenType::COLON, std::regex(":")),
  std::make_pair(TokenType::SEMICOLON, std::regex(";")),
  std::make_pair(TokenType::EQUALS, std::regex("=")),
  std::make_pair(TokenType::PUNCT, std::regex("[,.?!~&|\\-+*/%^@#]")),
  std::make_pair(TokenType::SPACE, std::regex("[ \\t\\v\\f]+")),
  std::make_pair(TokenType::EOL, std::regex("\\n")),
  std::make_pair(TokenType::WORD, std::regex("[^{}\\[\\]()<>'\":;=,.?!~&|\\-+*/%^:@#\\n \\t\\v\\f\\r]+"))
};

/**
 * Token.
 * 
 * A token is only valid for the lifetime of the Tokenizer that produced it,
 * as it contains a reference to a substring of the source file.
 */
struct Token {
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
