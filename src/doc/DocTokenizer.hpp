#pragma once

#include "doxide.hpp"
#include "doc/DocToken.hpp"

/**
 * DocTokenizer.
 */
class DocTokenizer {
public:
  /**
   * Constructor.
   * 
   * @param comment Comment to tokenize.
   */
  DocTokenizer(const std::string_view& source);
  
  /**
   * Get the next token.
   * 
   * @return Next token.
   * 
   * The token is only valid for the lifetime of the DocTokenizer, as it contains
   * a reference to a substring of the source file. If no tokens remain (i.e.
   * the end of the file is reached), a token with a type of `NONE` is
   * returned.
   */
  DocToken next();

  /**
   * Consume tokens until stopping criterion.
   * 
   * @param stop Bitmask giving the token type on which to stop and return.
   * 
   * @return Last token consumed.
   * 
   * DocTokens are consumed until one is encountered with a type in @p stop,
   * which is then returned. If @p stop is `ANY` then the next token is
   * returned.
   * 
   * @see interpret()
   */
  DocToken consume(const int stop = ANY);

private:
  /**
   * Iterator over source.
   */
  std::string_view::const_iterator iter;

  /**
   * End of source.
   */
  std::string_view::const_iterator end;
};
