#pragma once

#include "TextLineCursor.hpp"

#include <string_view>

/**
 * Documentation comment token types. Closing delimiters must be one shift
 * left of their opening counterparts.
 *
 * @ingroup developer
 */
enum DocTokenType : int {
  NONE = 0,
  OPEN_AFTER = 1 << 1,   // opening sequence for doc comment after entity
  OPEN_BEFORE = 1 << 2,  // opening sequence for doc comment before entity
  CLOSE = 1 << 3,        // closing sequence for doc comment
  COMMAND = 1 << 4,
  PARA = 1 << 5,
  LINE = 1 << 6,
  SENTENCE = 1 << 7,
  WHITESPACE = 1 << 8,
  WORD = 1 << 9,
  STAR = 1 << 10,
  SLASH = 1 << 11,
  ANY = ~0
};

/**
 * Token.
 *
 * @ingroup developer
 *
 * A token is only valid for the lifetime of the Tokenizer that produced it,
 * as it contains a reference to a substring of the source file.
 */
struct DocToken {
  /**
   * Constructor.
   *
   * @param type Token type.
   * @param value Token value.
   */
  DocToken(const DocTokenType type = NONE,
      TextLineCursor value = TextLineCursor());

  const TextLineCursor& get() const;

  /**
   * Get token as string.
   */
  std::string_view view() const;

  /**
   * Get substring of the token as a string.
   *
   * @param pos Position of the first character.
   */
  TextLineCursor substr(size_t pos = 0) const;

  /**
   * Token type.
   */
  DocTokenType type;

  /**
   * Iterator to first character.
   */
  TextLineCursor value;
};
