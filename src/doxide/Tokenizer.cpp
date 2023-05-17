#include "doxide/Tokenizer.hpp"

void Tokenizer::load(const std::string& file) {
  source.clear();
  char buffer[16384];
  std::ifstream in(file);
  while (in.read(buffer, sizeof(buffer))) {
    source.append(buffer, sizeof(buffer));
  }
  source.append(buffer, in.gcount());
  iter = source.begin();
}

Token Tokenizer::next() {
  Token token;
  if (iter != source.cend()) {
    for (auto& [type, regex] : regexes) {
      std::smatch match;
      if (std::regex_search(iter, source.cend(), match, regex,
          std::regex_constants::match_continuous)) {
        Token token{type, iter, iter + match.length()};
        iter += match.length();
        return token;
      }
    }
    error("unrecognized token starting: " << std::string(iter, iter + 20));
  }
  return token;
}
