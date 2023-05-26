#include "parser/Tokenizer.hpp"

Tokenizer::Tokenizer(const std::string_view& source) {
  iter = source.cbegin();
  end = source.cend();
}

Token Tokenizer::next() {
  Token token;
  if (iter != end) {
    for (auto& [type, regex] : regexes) {
      std::match_results<std::string_view::const_iterator> match;
      if (std::regex_search(iter, end, match, regex,
          std::regex_constants::match_continuous)) {
        Token token{type, iter, iter + match.length()};
        iter += match.length();
        return token;
      }
    }
    error("unrecognized token starting: " << std::string(iter, iter + 40) << "...");
  }
  return token;
}

Token Tokenizer::consume(const int stop) {
  Token token;
  do {
    token = next();
  } while (token.type && !(token.type & stop));
  return token;
}
