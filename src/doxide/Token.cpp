#include "doxide/Token.hpp"

Token::Token(const TokenType& type, const std::string::const_iterator& first,
    const std::string::const_iterator& last) :
    type(type),
    first(first),
    last(last) {
  //
}

std::string_view Token::str() const {
  return std::string_view(first, last);
}
