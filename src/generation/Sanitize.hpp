#pragma once

#include <Regex.hpp>
#include <entities/FwdRef.hpp>
#include <entities/Entities.hpp>

#include <concepts>
#include <iterator>
#include <format>
#include <regex>
#include <string>
#include <string_view>

class Documentable;
class Titled;

template<entity U> constexpr std::string_view get_title(const FwdRef<U>& e) {
  if constexpr (std::derived_from<U, Titled>) {
    return e->get_title();
  } else {
    return e->get_name();
  }
}

struct html_escaped {
  std::string_view text;
};

template <>
struct std::formatter<html_escaped, char> {
  template <class ParseContext> constexpr auto parse(ParseContext& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it != '}')
      throw std::format_error("html_escaped does not take a format spec");
    return it;
  }

  template <class FormatContext>
  auto format(const html_escaped& h, FormatContext& ctx) const {
    static constexpr std::string_view operator_brackets = "operator[]";

    auto out = ctx.out();
    const std::string_view s = h.text;

    for (std::size_t i = 0; i < s.size(); ++i) {
      if (s.substr(i).starts_with(operator_brackets)) {
        out = std::ranges::copy(std::string_view{"operator&#91;&#93;"}, out).out;
        i += operator_brackets.size() - 1;
        continue;
      }

      std::string_view rep;
      switch (s[i]) {
        case '&':  rep = "&amp;";  break;
        case '<':  rep = "&lt;";   break;
        case '>':  rep = "&gt;";   break;
        case '"':  rep = "&quot;"; break;
        case '\'': rep = "&apos;"; break;
        case '*':  rep = "&#42;";  break;
        default:   *out++ = s[i];  continue;
      }
      out = std::ranges::copy(rep, out).out;
    }
    return out;
  }
};

/* Optional: keep the old API as a thin wrapper. */
inline std::string htmlize(std::string_view str) {
  return std::format("{}", html_escaped{str});
}

struct sanitized {
  std::string_view text;
};

template<> struct std::formatter<sanitized, char> {
  static constexpr std::size_t MAX_SIZE = 250;

  enum class CHAR_TYPE {SPACE, ALNUM, OTHER};

  template <class ParseContext> constexpr auto parse(ParseContext& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it != '}')
      throw std::format_error("sanitized does not take a format spec");
    return it;
  }

  template <class FormatContext>
  auto format(const sanitized& h, FormatContext& ctx) const {
    auto out = ctx.out();
    const std::string_view s = h.text;
    std::size_t out_size = 0;
    std::array<char, 6> buffer;
    // std::array<char, 8> buffer;

    for (const unsigned char c : s) {
      switch (lookup[c]) {
        case CHAR_TYPE::SPACE: break;
        case CHAR_TYPE::ALNUM: {
          *out++ = c;
          out_size++;
          break;
        }
        case CHAR_TYPE::OTHER: {
          // the original implementation uses signed int and pads to 8 bytes
          // std::format_to(buffer.begin(), "_u{:04x}", static_cast<signed int>(c));
          std::format_to(buffer.begin(), "_u{:04x}", c);
          std::size_t remaining = MAX_SIZE - out_size;
          std::size_t to_print = std::min(remaining, buffer.size());
          out = std::copy_n(buffer.begin(), to_print, out);
          out_size += to_print;
          break;
        }
      }
      if (out_size == MAX_SIZE) {
        return out;
      }
    }
    return out;
  }
private:
  // compile-time initialized and declared in .cpp to spare compilation time
  static std::array<CHAR_TYPE, 256> lookup;
};

inline std::string sanitize(const std::string_view& str) {
  return std::format("{}", sanitized(str));
}

struct liner {
  std::string_view text;
};

template<> struct std::formatter<liner, char> {
  template <class ParseContext> constexpr auto parse(ParseContext& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it != '}')
      throw std::format_error("liner does not take a format spec");
    return it;
  }

  template <class FormatContext>
  auto format(const liner& h, FormatContext& ctx) const {
    static const std::regex newline("\\s*\\n\\s*", REGEX_FLAGS);
    auto out = ctx.out();
    return std::regex_replace(out, h.text.cbegin(), h.text.cend(), newline, " ");
  }
};

inline std::string line(std::string_view str) {
  return std::format("{}", liner(str));
}

template<std::derived_from<Documentable> T> std::string_view brief(const FwdRef<T>& entity) {
  static const std::regex reg("^(`.*?`|\\[.*?\\]\\(.*?\\)|[^;:.?!])*[\\.\\?\\!](?=\\s|$)", REGEX_FLAGS);
  std::string_view docs = entity->get_docs();
  std::match_results<std::string_view::const_iterator> match;
  std::string_view matched_substr =
    std::regex_search(docs.cbegin(), docs.cend(), match, reg) ?
    std::string_view{match[0].first, match[0].second} : docs;
  return matched_substr;
}
