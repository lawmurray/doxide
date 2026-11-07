#pragma once

#include <Regex.hpp>
#include <entities/FwdRef.hpp>
#include <entities/Entities.hpp>

#include <concepts>
#include <regex>
#include <string>
#include <string_view>

class Documentable;
class Titled;

std::string sanitize(const std::string_view str);

std::string line(std::string_view str);

// get first sentence
template<std::derived_from<Documentable> T> std::string brief(const FwdRef<T>& entity) {
  static const std::regex reg("^(`.*?`|\\[.*?\\]\\(.*?\\)|[^;:.?!])*[\\.\\?\\!](?=\\s|$)", REGEX_FLAGS);
  std::string l = line(entity->get_docs());
  std::smatch match;
  if (std::regex_search(l, match, reg)) {
    return match.str();
  } else {
    return l;
  }
}

std::string htmlize(const std::string& str);

std::string indent(const std::string& str);

template<entity U> constexpr std::string_view get_title(const FwdRef<U>& e) {
  if constexpr (std::derived_from<U, Titled>) {
    return e->get_title();
  } else {
    return e->get_name();
  }
}
