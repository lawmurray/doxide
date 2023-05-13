#include "doxide/Tokenizer.hpp"

static auto regexes = {
  std::make_pair(TokenType::DOC_COMMENT_OPEN, std::regex("/\\*\\*")),
  std::make_pair(TokenType::DOC_COMMENT_CLOSE, std::regex("\\*/")),
  std::make_pair(TokenType::INLINE_COMMENT_OPEN, std::regex("/\\*")),
  std::make_pair(TokenType::INLINE_COMMENT_CLOSE, std::regex("\\*/")),
  std::make_pair(TokenType::LINE_COMMENT, std::regex("//.*?$")),
  std::make_pair(TokenType::BRACE_OPEN, std::regex("\\{")),
  std::make_pair(TokenType::BRACE_CLOSE, std::regex("\\}")),
  std::make_pair(TokenType::BRACKET_OPEN, std::regex("\\[")),
  std::make_pair(TokenType::BRACKET_CLOSE, std::regex("\\]")),
  std::make_pair(TokenType::PAREN_OPEN, std::regex("\\(")),
  std::make_pair(TokenType::PAREN_CLOSE, std::regex("\\)")),
  std::make_pair(TokenType::ANGLE_OPEN, std::regex("<")),
  std::make_pair(TokenType::ANGLE_CLOSE, std::regex(">")),
  std::make_pair(TokenType::CHAR, std::regex("'(?:\\\\'|[^'])*?'")),
  std::make_pair(TokenType::STRING, std::regex("\"(?:\\\\\"|[^\"])*?\"")),
  std::make_pair(TokenType::COLON, std::regex(":")),
  std::make_pair(TokenType::SEMICOLON, std::regex(";")),
  std::make_pair(TokenType::NAMESPACE, std::regex("\\bnamespace\\b")),
  std::make_pair(TokenType::CLASS, std::regex("\\bclass\\b")),
  std::make_pair(TokenType::STRUCT, std::regex("\\bstruct\\b")),
  std::make_pair(TokenType::ENUM, std::regex("\\benum\\b")),
  std::make_pair(TokenType::USING, std::regex("\\busing\\b")),
  std::make_pair(TokenType::FRIEND, std::regex("\\busing\\b")),
  std::make_pair(TokenType::END_OF_LINE, std::regex("\\n")),
  std::make_pair(TokenType::SPACE, std::regex("[ \\t\\v\\f]+")),
  std::make_pair(TokenType::OTHER, std::regex("[^{}\\[\\]()<>'\":; \\t\\v\\f\\n\\r]+"))
};

Token Tokenizer::next() {
  if (valueIter == keyIter->second.cend()) {
    Token token{TokenType::END_OF_FILE, valueIter, valueIter};
    ++keyIter;
    currentLine = 1;
    if (keyIter != sources.cend()) {
      valueIter = keyIter->second.cbegin();
    }
    return token;
  } else {
    std::smatch match;
    for (auto& regex : regexes) {
      if (std::regex_search(valueIter, keyIter->second.cend(), match,
          regex.second, std::regex_constants::match_continuous)) {
        Token token{regex.first, valueIter, valueIter + match.length()};
        valueIter += match.length();
        if (regex.first == TokenType::END_OF_LINE) {
          ++currentLine;
          currentColumn = 0;
        } else {
          currentColumn += match.length();
        }
        return token;
      }
    }
    error("unrecognized token");
  }
}
