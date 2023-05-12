#include "doxide/Tokenizer.hpp"

static auto regexes = {
  std::make_pair(DOC_COMMENT, std::regex("/\\*\\*(?:.|\\n)*?\\*/")),
  std::make_pair(INLINE_COMMENT, std::regex("/\\*(?:.|\\n)*?\\*/")),
  std::make_pair(LINE_COMMENT, std::regex("//.*?$")),
  std::make_pair(BRACE_OPEN, std::regex("\\{")),
  std::make_pair(BRACE_CLOSE, std::regex("\\}")),
  std::make_pair(BRACKET_OPEN, std::regex("\\[")),
  std::make_pair(BRACKET_CLOSE, std::regex("\\]")),
  std::make_pair(PAREN_OPEN, std::regex("\\(")),
  std::make_pair(PAREN_CLOSE, std::regex("\\)")),
  std::make_pair(ANGLE_OPEN, std::regex("<")),
  std::make_pair(ANGLE_CLOSE, std::regex(">")),
  std::make_pair(CHAR, std::regex("'(?:\\\\'|[^'])'")),
  std::make_pair(STRING, std::regex("\"(?:\\\\\"|[^\"])*?\"")),
  std::make_pair(COLON, std::regex(":")),
  std::make_pair(SEMICOLON, std::regex(";")),
  std::make_pair(NAMESPACE, std::regex("namespace")),
  std::make_pair(CLASS, std::regex("class")),
  std::make_pair(STRUCT, std::regex("struct")),
  std::make_pair(END_OF_LINE, std::regex("\\n")),
  std::make_pair(SPACE, std::regex("\\s+")),
  std::make_pair(OTHER, std::regex("[^{}\\[\\]()<>'\":; \t\v\f\n\r]+"))
};

Token Tokenizer::next() {
  if (valueIter == keyIter->second.cend()) {
    Token token(END_OF_FILE, valueIter, valueIter);
    ++keyIter;
    if (keyIter != sources.cend()) {
      valueIter = keyIter->second.cbegin();
    }
    return token;
  } else {
    std::smatch match;
    for (auto& regex : regexes) {
      if (std::regex_search(valueIter, keyIter->second.cend(), match,
          regex.second, std::regex_constants::match_continuous)) {
        Token token(regex.first, valueIter, valueIter + match.length());
        valueIter += match.length();
        return token;
      }
    }
    error("unrecognized token");
  }
}
