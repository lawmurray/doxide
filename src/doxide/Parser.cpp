#include "doxide/Parser.hpp"
#include "doxide/Tokenizer.hpp"

void Parser::parse(const std::string& file) {
  Tokenizer tokenizer(file);
  while (tokenizer.hasNext()) {
    Token token = tokenizer.next();
  }
}