#include "doxide/Tokenizer.hpp"

void Tokenizer::load(const std::string& file) {
  std::stringstream stream;
  stream << std::ifstream(file).rdbuf();
  source = stream.str();
  iter = source.begin();
}

Token Tokenizer::next() {
  Token token;
  if (iter != source.cend()) {
    std::smatch match;
    for (auto& [type, regex] : regexes) {
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
