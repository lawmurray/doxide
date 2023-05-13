#pragma once

#include "doxide.hpp"
#include "doxide/Token.hpp"

/**
 * Tokenizer.
 */
class Tokenizer {
public:
  /**
   * Constructor.
   * 
   * @param file File name.
   */
  Tokenizer();

  /**
   * Load a file.
   */
  void load(const std::string& file);

  /**
   * Get the current file.
   */
  const std::string& file() const;

  /**
   * Get the current line in the current file.
   */
  size_t line() const;

  /**
   * Get the current column in the current file.
   */
  size_t column() const;

  /**
   * Is there another token?
   */
  bool hasNext() const;
  
  /**
   * Get the next token.
   * 
   * @return Next token.
   * 
   * The token is only valid for the lifetime of the Tokenizer, as it contains
   * a reference to a substring of the source file.
   */
  Token next();

private:
  /**
   * File name.
   */
  std::string currentFile;

  /**
   * Current line in current file.
   */
  size_t currentLine;

  /**
   * Current column in current file.
   */
  size_t currentColumn;

  /**
   * File contents.
   */
  std::string source;

  /**
   * Iterator over file contents.
   */
  std::string::const_iterator iter;
};
