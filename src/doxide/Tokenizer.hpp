#pragma once

#include "doxide.hpp"
#include "doxide/Token.hpp"

/**
 * Tokenizer.
 */
class Tokenizer {
public:
  /**
   * Load a file.
   */
  void load(const std::string& file);
  
  /**
   * Get the next token.
   * 
   * @return Next token.
   * 
   * The token is only valid for the lifetime of the Tokenizer, as it contains
   * a reference to a substring of the source file. If no tokens remain (i.e.
   * the end of the file is reached), a token with a type of `NONE` is
   * returned.
   */
  Token next();

private:
  /**
   * File contents.
   */
  std::string source;

  /**
   * Iterator over file contents.
   */
  std::string::const_iterator iter;
};
