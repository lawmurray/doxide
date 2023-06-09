#include "parser/Tokenizer.hpp"

Tokenizer::Tokenizer(const std::string_view& source) {
  iter = source.cbegin();
  end = source.cend();
}

Token Tokenizer::next() {
  Token token(NONE, iter, iter);
  if (iter != end) {
    for (auto& [type, regex] : regexes) {
      std::match_results<std::string_view::const_iterator> match;
      if (std::regex_search(iter, end, match, regex,
          std::regex_constants::match_continuous)) {
        iter += match.length();
        token.type = type;
        token.last = iter;
        return token;
      }
    }
    error("unrecognized token starting: " << std::string(iter, iter + 40) << "...");
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
