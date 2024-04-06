#include "Token.hpp"

Token::Token(const TokenType& type,
    const std::string_view::const_iterator& first,
    const std::string_view::const_iterator& last) :
    type(type),
    first(first),
    last(last) {
  //
}

std::string_view Token::str() const {
  /* std::string_view(first, last) ought to work with C++20 support, but
    * using the below overload of the constructor extends support to some
    * older compilers, such as gcc with gnu++2a support only, and MSVC 2022 */
  return std::string_view(&*first, std::distance(first, last));
}

std::string_view Token::substr(size_t pos) const {
  return str().substr(pos);
}
