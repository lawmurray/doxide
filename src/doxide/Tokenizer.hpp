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
    std::stringstream value;
    for (auto iter = first; iter != last; ++iter) {
      std::ifstream in(*iter);
      value << in.rdbuf();
      sources.insert({*iter, value.str()});
      value.str("");
    }

    /* initialize iterator over files */
    keyIter = sources.cbegin();

    /* initialize iterator over file contents */
    valueIter = keyIter->second.cbegin();
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
};
