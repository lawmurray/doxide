#include <TextLineCursor.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>

TextLineCursor::TextLineCursor(const std::string_view &v): TextLineCursor(v, 0) {}

void TextLineCursor::advance(std::size_t length) {
  check_length(length);
  std::string_view subs = _data.substr(0, length);
  _line_num += _do_count_lines_in_range(subs.cbegin(), subs.end());
  _data = _data.substr(length);
}

void TextLineCursor::clear() noexcept {
  _data = std::string_view();
  _line_num = 0;
}

void TextLineCursor::check_start(const_iterator start) const {
  if (start < cbegin()) {
    throw std::out_of_range("start is before beginning");
  }
  if (start > cend()) {
    throw std::out_of_range("start is beyond size");
  }
}

void TextLineCursor::check_length(std::size_t length) const {
  if (length > size()) {
    throw std::out_of_range("length is beyond size");
  }
}

std::size_t TextLineCursor::count_lines_in_range(const_iterator start, const_iterator end) const {
  return _do_count_lines_in_range(start, end);
}

std::size_t TextLineCursor::count_lines_in_range(const_iterator start, std::size_t length) const {
  std::string_view subs = _data.substr(std::distance(_data.cbegin(), start), length);
  return count_lines_in_range(start, subs.cend());
}

std::size_t TextLineCursor::count_lines_in_range(std::size_t start, std::size_t length) const {
  std::string_view subs = _data.substr(start, length);
  return count_lines_in_range(subs.cbegin(), subs.cend());
}

std::size_t TextLineCursor::_do_count_lines_in_range(const_iterator start, const_iterator end) const {
  return std::count(start, end, '\n');
}

TextLineCursor TextLineCursor::substr(std::size_t start, std::size_t size) const {
  auto sub = _data.substr(start, size);
  auto res = TextLineCursor(sub, _line_num + count_lines_in_range(cbegin(), start));
  return res;
}

TextLineCursor TextLineCursor::substr(std::size_t start) const {
  std::size_t len = size() - start;
  auto sub = _data.substr(start, len);
  auto res = TextLineCursor(sub, _line_num + count_lines_in_range(cbegin(), start));
  return res;
}

TextLineCursor TextLineCursor::substr(const_iterator start, std::size_t length) const {
  auto sub = _data.substr(std::distance(cbegin(), start), length);
  return TextLineCursor(sub, _line_num + count_lines_in_range(cbegin(), start));
}
