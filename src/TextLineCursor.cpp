#include <TextLineCursor.hpp>
#include <algorithm>
#include <stdexcept>

TextLineCursor::TextLineCursor(): _start(nullptr), _size(0), _line_num(0) {}

TextLineCursor::TextLineCursor(const char* start, std::size_t size, std::size_t line_num):
  _start(start), _size(size), _line_num(line_num) {}

TextLineCursor::TextLineCursor(const std::string_view &v): TextLineCursor(v.data(), v.size(), 0) {}

TextLineCursor::TextLineCursor(const std::string &v) : TextLineCursor(v.data(), v.size(), 0) {}

void TextLineCursor::advance(std::size_t length) {
  check_length(length);
  _line_num += _do_count_lines_in_range(cbegin(), cbegin() + length);
  _start += length;
  _size -= length;
}

void TextLineCursor::clear() noexcept {
  _start = nullptr;
  _size = 0;
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

void TextLineCursor::check_end(const_iterator end) const {
  if (end > cend()) {
    throw std::out_of_range("end is beyond text");
  }
}

void TextLineCursor::check_range(std::size_t start, std::size_t length) const {
  check_start(cbegin() + start);
  check_length(start + length);
}

void TextLineCursor::check_range(const_iterator start, std::size_t length) const {
  check_start(start);
  check_end(start + length);
}

void TextLineCursor::check_range(const_iterator start, const_iterator end) const {
  if (end < start) {
    throw std::out_of_range("end is before start");
  }
  check_start(start);
  check_end(end);
}

std::size_t TextLineCursor::count_lines_in_range(const_iterator start, const_iterator end) const {
  check_range(start, end);
  return _do_count_lines_in_range(start, end);
}

std::size_t TextLineCursor::count_lines_in_range(const_iterator start, std::size_t length) const {
  return count_lines_in_range(start, start + length);
}

std::size_t TextLineCursor::count_lines_in_range(std::size_t start, std::size_t length) const {
  const char* _begin = _start + start;
  return count_lines_in_range(_begin, _begin + length);
}

std::size_t TextLineCursor::_do_count_lines_in_range(const_iterator start, const_iterator end) const {
  return std::count(start, end, '\n');
}

TextLineCursor TextLineCursor::substr(std::size_t start, std::size_t size) const {
  check_range(start, size);
  auto res = TextLineCursor(cbegin() + start, size, _line_num + count_lines_in_range(cbegin(), start));
  return res;
}

TextLineCursor TextLineCursor::substr(std::size_t start) const {
  std::size_t len = size() - start;
  check_range(start, len);
  auto res = TextLineCursor(cbegin() + start, len, _line_num + count_lines_in_range(cbegin(), start));
  return res;
}

TextLineCursor TextLineCursor::substr(const_iterator start, std::size_t length) const {
  check_range(start, length);
  return TextLineCursor(start, length, _line_num + count_lines_in_range(cbegin(), start));
}
