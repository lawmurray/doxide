#pragma once

#include <format>
#include <tuple>

template<class ParseContext>
constexpr std::tuple<typename ParseContext::iterator, bool> __parse(ParseContext& ctx) {
  bool dbg = false;
  auto it = ctx.begin();
  auto end = ctx.end();
  if (it == end) {
    return std::make_tuple(it, dbg);
  }
  if (*it == '?') {
    dbg = true;
    ++it;
  }
  if (it != end and *it != '}') {
    throw std::format_error("Invalid format args.");
  }
  return std::make_tuple(it, dbg);
}

struct StatelessEntityFormatter {
  template<class ParseContext> constexpr ParseContext::iterator parse(ParseContext& ctx) {
    return std::get<0>(__parse<ParseContext>(ctx));
  }
};

struct StatefulEntityFormatter {
  bool dbg = false;
  void set_dbg() { dbg = true; }
  template<class ParseContext> constexpr ParseContext::iterator parse(ParseContext& ctx) {
    auto [it, has_dbg] = __parse<ParseContext>(ctx);
    dbg = has_dbg;
    return it;
  }
};
