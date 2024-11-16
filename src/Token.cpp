#include "Token.hpp"

Token::Token(const TokenType type, const std::string_view value) :
    type(type),
    value(value) {
  //
}

std::string_view Token::str() const {
  /* std::string_view(first, last) ought to work with C++20 support, but
   * using the below overload of the constructor extends support to some
   * older compilers, such as gcc with gnu++2a support only, and MSVC 2022 */
  return value;
}

std::string_view Token::substr(size_t pos) const {
  return value.substr(pos);
}
