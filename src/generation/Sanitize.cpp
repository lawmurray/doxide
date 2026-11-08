#include "Sanitize.hpp"

static consteval std::array<std::formatter<sanitized, char>::CHAR_TYPE, 256>
  _create_lookup_table() {
  std::array<std::formatter<sanitized, char>::CHAR_TYPE, 256> table{};
  for (int i = 0; i < 256; ++i) {
    char c = static_cast<char>(i);
    if (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r') {
      table[i] = std::formatter<sanitized, char>::CHAR_TYPE::SPACE;
    } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
          c == '_' || c == '.' || c == '/' || c == '\\') {
      table[i] = std::formatter<sanitized, char>::CHAR_TYPE::ALNUM;
    } else {
      table[i] = std::formatter<sanitized, char>::CHAR_TYPE::OTHER;
    }
  }
  return table;
}

constinit std::array<std::formatter<sanitized, char>::CHAR_TYPE, 256>
  std::formatter<sanitized, char>::lookup = _create_lookup_table();
