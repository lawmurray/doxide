#include "Tokenizer.hpp"

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
        token.type = type;
        token.value = std::string_view(iter, iter + match.length());
        iter += match.length();
        return token;
      }
    }
    error("unrecognized token");
  }
  return token;
}

Token Tokenizer::consume(const int stop) {
  Token token = next();
  while (token.type && !(token.type & stop)) {
    token = next();
  }    
  return token;
}
