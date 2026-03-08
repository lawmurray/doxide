#include "DocTokenizer.hpp"

DocTokenizer::DocTokenizer(const TextLineCursor& src): source(src) {}

DocToken DocTokenizer::next() {
  DocToken token;
  auto iter = source.cbegin();
  auto end = source.cend();
  if (iter != end) {
    for (auto& [type, regex] : regexes) {
      std::match_results<TextLineCursor::const_iterator> match;
      if (std::regex_search(iter, end, match, regex,
          std::regex_constants::match_continuous)) {
        token.type = type;
        // token.value = std::string_view(iter, iter + match.length());
        token.value = source.substr(iter, match.length());
        // iter += match.length();
        source.advance(match.length());
        return token;
      }
    }
    error("unrecognized token");
  }
  return token;
}

DocToken DocTokenizer::consume(const int stop) {
  DocToken token = next();
  while (token.type && !(token.type & stop)) {
    token = next();
  }
  return token;
}
