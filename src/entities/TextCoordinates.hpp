#pragma once

#include <cstddef>
#include <string_view>
#include <utility>

struct TextCoordinates: protected std::pair<std::size_t, std::size_t> {
  using std::pair<std::size_t, std::size_t>::pair;

  constexpr std::size_t offset() const noexcept { return this->first; }

  constexpr std::size_t len() const noexcept { return this->second; }

  constexpr std::string_view within(std::string_view v) const {
    return v.substr(offset(), len());
  }
};
