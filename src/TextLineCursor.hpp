#pragma once

#include <string_view>
#include <string>
#include <ostream>
#include <utility>

/**
 * Allows cursoring through external text keeping track of the line number.
 *
 * @ingroup developer
 */
class TextLineCursor {
public:

  using iterator = const char*;
  using const_iterator = const char*;

  /**
   * Construct an empty cursor.
   */
  TextLineCursor();

  /**
   * Constructor from string_view.
   *
   * @param v text tu cursor on.
   */
  explicit TextLineCursor(const std::string_view &v);

  /**
   * Constructor from string.
   *
   * @param v text tu cursor on.
   */
  explicit TextLineCursor(const std::string &v);

  /**
   * Constructor from string_view.
   *
   * @param v text tu cursor on.
   */
  TextLineCursor(const TextLineCursor &) = default;

  TextLineCursor& operator=(const TextLineCursor &) = default;

  inline char operator[](std::size_t i) const noexcept { return _start[i]; }

  /**
   * Get the pointer to the underlying data.
   */
  inline const char* data() const noexcept { return _start; }

  /**
   * Get the number of the current line: *it is 0-based*.
   */
  inline std::size_t get_line_number() const noexcept { return _line_num; }

  /**
   * Get the number of characters in the text.
   */
  inline std::size_t size() const noexcept { return _size; }

  /**
   * Get the text as a string_view.
   */
  inline std::string_view view() const noexcept {
    return std::string_view(data(), size());
  }

  inline bool empty() const noexcept { return size() == 0; }

  /**
   * Creates a (separate) sub-text of the current text.
   *
   * @param start start offset of the new sub-text
   * @param length length of the new sub-text
   */
  TextLineCursor substr(std::size_t start, std::size_t length) const;

  /**
   * Creates a (separate) sub-text of the current text from the given start
   * to the current length of the text.
   *
   * @param start start offset of the new sub-text
   */
  TextLineCursor substr(std::size_t start) const;

  /**
   * Creates a (separate) sub-text of the current text from a start position
   * (passed as an iterator) and for a given length.
   *
   * @param start start position of the new sub-text
   * @param length length of the new sub-text
   * @return TextLineCursor
   */
  TextLineCursor substr(const_iterator start, std::size_t length) const;

  inline const char* begin() const noexcept { return _start; }

  inline const char* cbegin() const noexcept { return _start; }

  inline const char* end() const noexcept { return _start + _size; }

  inline const char* cend() const noexcept { return _start + _size; }

  /**
   * Converst to string.
   */
  inline std::string to_string() const noexcept { return std::string(cbegin(), cend()); }

  /**
   * Advance the cursor by length characters; it throws out_of_range if `length`
   * is higher that `size()`.
   */
  void advance(std::size_t length);

  /**
   * Clears the current cursor.
   */
  void clear() noexcept;

private:
  const char* _start;
  std::size_t _size;
  std::size_t _line_num;

  TextLineCursor(const char* start, std::size_t size, std::size_t line_start);

  void check_start(const_iterator start) const;

  void check_length(std::size_t length) const;

  void check_end(const_iterator end) const;

  void check_range(std::size_t start, std::size_t length) const;

  void check_range(const_iterator start, std::size_t length) const;

  void check_range(const_iterator start, const_iterator end) const;

  std::size_t count_lines_in_range(const_iterator start, const_iterator end) const;

  std::size_t count_lines_in_range(const_iterator start, std::size_t length) const;

  std::size_t count_lines_in_range(std::size_t start, std::size_t length) const;

  // unsafe version, without boundaries checks
  std::size_t _do_count_lines_in_range(const_iterator start, const_iterator end) const;
};

/**
 * Compares the text in the view `v` against that in the cursor `w`.
 */
inline bool operator==(const std::string_view& v, const TextLineCursor& w) { return v == w.view(); }

/**
 * Allows printing the cursor `v` into the stream `s`.
 */
inline std::ostream& operator<<(std::ostream& s, const TextLineCursor& v) { return s << v.view(); }
