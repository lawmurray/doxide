#include "doxide/Tokenizer.hpp"

Tokenizer::Tokenizer() :
    currentLine(0),
    currentColumn(0) {
  //
}

void Tokenizer::load(const std::string& file) {
  currentFile = file;
  currentLine = 1;
  currentColumn = 0;

  std::stringstream stream;
  stream << std::ifstream(file).rdbuf();
  source = stream.str();
  iter = source.begin();
}

const std::string& Tokenizer::file() const {
  return currentFile;
}

size_t Tokenizer::line() const {
  return currentLine;
}

size_t Tokenizer::column() const {
  return currentColumn;
}

Token Tokenizer::next() {
  Token token;
  if (iter == source.cend()) {
    return token;
  } else {
    std::smatch match;
    for (auto& regex : regexes) {
      if (std::regex_search(iter, source.cend(), match, regex.second,
          std::regex_constants::match_continuous)) {
        Token token{regex.first, iter, iter + match.length()};
        iter += match.length();
        if (regex.first & EOL) {
          currentLine += 1;
          currentColumn = 0;
        } else {
          currentColumn += match.length();
        }
        return token;
      }
    }
    warn("unrecognized token at " << currentFile << ':' << currentLine << ':'
        << currentColumn);
    return token;
  }
}
