#pragma once

#include <ostream>
#include <format>
#include <iterator>
#include <iostream>
#include <utility>

template<typename stream_t, typename iter_t> class _sink_t {
public:
  _sink_t(stream_t& s): _it(s) {}

  template<class... Args> iter_t print(std::format_string<Args...> fmt, Args&&... args ) {
    _it = std::format_to(_it, fmt, std::forward<Args>(args)...);
    return _it;
  }

  template<class... Args> iter_t println(std::format_string<Args...> fmt, Args&&... args ) {
    iter_t it = this->print(fmt, std::forward<Args>(args)...);
    *it = '\n';
    ++_it;
    return it;
  }

  inline iter_t println() {
    *_it = '\n';
    ++_it;
    return _it;
  }

  iter_t& iter() { return _it; }

private:
  iter_t _it;
};

class sink_t: public _sink_t<std::ostream, std::ostream_iterator<char>> {
public:
  using _sink_t<std::ostream, std::ostream_iterator<char>>::_sink_t;

  sink_t(): sink_t(std::cout) {}
};
