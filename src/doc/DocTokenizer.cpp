#include "doc/DocTokenizer.hpp"

DocTokenizer::DocTokenizer(const std::string_view& source) {
  iter = source.cbegin();
  end = source.cend();
}

DocToken DocTokenizer::next() {
  DocToken token;
  if (iter != end) {
    for (auto& [type, regex] : regexes) {
      std::match_results<std::string_view::const_iterator> match;
      if (std::regex_search(iter, end, match, regex,
          std::regex_constants::match_continuous)) {
        DocToken token{type, iter, iter + match.length()};
        iter += match.length();
        return token;
      }
    }
    error("unrecognized token starting: " << std::string(iter, iter + 40) << "...");
  }
  return token;
}

DocToken DocTokenizer::consume(const int stop) {
  DocToken token;
  do {
    token = next();
  } while (token.type && !(token.type & stop));
  return token;
}
