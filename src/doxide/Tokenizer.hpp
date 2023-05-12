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
   * @param first Iterator to first file name.
   * @param last Iterator to last file name.
   */
  template<class Iterator>
  Tokenizer(Iterator first, Iterator last) {
    /* read all file contents */
    for (auto iter = first; iter != last; ++iter) {
      std::stringstream value;
      std::ifstream in(*iter);
      value << in.rdbuf();
      sources.insert({*iter, value.str()});
    }

    keyIter = sources.cbegin();
    valueIter = keyIter->second.cbegin();
    currentLine = 1;
    currentColumn = 0;
  }

  /**
   * Is there another token?
   */
  bool hasNext() const {
    return keyIter != sources.cend();
  }
  
  /**
   * Get the next token.
   */
  Token next();

  /**
   * Get the current file.
   */
  const std::string& file() const {
    return keyIter->first;
  }

  /**
   * Get the current line in the current file.
   */
  size_t line() const {
    return currentLine;
  }

  /**
   * Get the current column in the current file.
   */
  size_t column() const {
    return currentColumn;
  }

private:
  using map_type = std::unordered_map<std::string,std::string>;
  using iterator_type = map_type::const_iterator;
  using value_iterator_type = std::string::const_iterator;

  /**
   * Sources. Keys are file names, values are contents. Individual tokens
   * create std::string_view over these values.
   */
  map_type sources;

  /**
   * Iterator through source files.
   */
  iterator_type keyIter;

  /**
   * Iterator through current source file contents.
   */
  value_iterator_type valueIter;

  /**
   * Current line in current file.
   */
  size_t currentLine;

  /**
   * Current column in current file.
   */
  size_t currentColumn;
};
