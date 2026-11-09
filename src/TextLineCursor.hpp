#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <string_view>

/**
 * Allows cursoring through external text keeping track of the line number.
 *
 * @ingroup developer
 */
class TextLineCursor {
public:

  using iterator = std::string_view::iterator;
  using const_iterator = std::string_view::const_iterator;

  /**
   * Construct an empty cursor.
   */
  constexpr TextLineCursor(): _data(), _line_num(0) {}

  /**
   * Constructor from string_view.
   *
   * @param v text tu cursor on.
   */
  explicit TextLineCursor(const std::string_view &v);

  /**
   * Copy constructor.
   *
   * @param v text tu cursor on.
   */
  TextLineCursor(const TextLineCursor &) = default;

  TextLineCursor(TextLineCursor&&) = default;

  TextLineCursor& operator=(const TextLineCursor &) = default;

  inline char operator[](std::size_t i) const {
    return _data[i];
  }

  /**
   * Get the pointer to the underlying data.
   */
  // inline const char* data() const noexcept { return _data.data(); }

  /**
   * Get the number of the current line: *it is 0-based*.
   */
  inline std::size_t get_line_number() const noexcept { return _line_num; }

  /**
   * Get the number of characters in the text.
   */
  inline std::size_t size() const noexcept { return _data.size(); }

  /**
   * Get the text as a string_view.
   */
  constexpr inline std::string_view view() const noexcept {
    return _data;
  }

  inline constexpr operator std::string_view() const noexcept { return view();}

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

  inline auto begin() const noexcept { return _data.begin(); }

  inline auto cbegin() const noexcept { return _data.begin(); }

  inline auto end() const noexcept { return _data.end(); }

  inline auto cend() const noexcept { return _data.end(); }

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
  std::string_view _data;
  std::size_t _line_num;

  constexpr TextLineCursor(std::string_view data, std::size_t line_start):
    _data(data), _line_num(line_start) {}

  void check_start(const_iterator start) const;

  void check_length(std::size_t length) const;

  std::size_t count_lines_in_range(const_iterator start, const_iterator end) const;

  std::size_t count_lines_in_range(const_iterator start, std::size_t length) const;

  std::size_t count_lines_in_range(std::size_t start, std::size_t length) const;

  // unsafe version, without boundaries checks
  std::size_t _do_count_lines_in_range(const_iterator start, const_iterator end) const;
};

/**
 * Compares the text in the view `v` against that in the cursor `w`.
 *
 * @ingroup developer
 */
inline bool operator==(const std::string_view& v, const TextLineCursor& w) { return v == w.view(); }

/**
 * Allows printing the cursor `v` into the stream `s`.
 *
 * @ingroup developer
 */
inline std::ostream& operator<<(std::ostream& s, const TextLineCursor& v) { return s << v.view(); }
