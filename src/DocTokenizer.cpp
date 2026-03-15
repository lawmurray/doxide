#include "DocTokenizer.hpp"
#include "Regex.hpp"
#include "Log.hpp"

DocTokenizer::DocTokenizer(const TextLineCursor& src): source(src) {}

/**
 * Documentation comment token patterns. Order is important, as a match to an
 * earlier pattern precludes a match to a later.
 *
 * @ingroup developer
 */
static auto regexes = {
  std::make_pair(OPEN_AFTER, std::regex("(?:/\\*\\*|/\\*!|///|//!)<[ \\t]?", REGEX_FLAGS)),
  std::make_pair(OPEN_BEFORE, std::regex("(?:/\\*\\*|/\\*!|///|//!)[ \\t]?", REGEX_FLAGS)),
  std::make_pair(CLOSE, std::regex("\\*/", REGEX_FLAGS)),
  std::make_pair(COMMAND, std::regex("[@\\\\](?:param(?:\\[(?:in|out|in,out)\\])?|\\w+|@|\\\\|/|f[\\$\\[\\]])", REGEX_FLAGS)),

  /* the end of a paragraph is two new lines  */
  std::make_pair(PARA, std::regex("(?:[ \\t]*\\n(?:[ \\t]*\\*(?!/))?[ \\t]?){2}", REGEX_FLAGS)),

  /* the end of a line is one new line, as long as there is not an end of
   * comment to come */
  std::make_pair(LINE, std::regex("[ \\t]*\\n(?:[ \\t]*\\*(?!/))?[ \\t]?", REGEX_FLAGS)),

  std::make_pair(SENTENCE, std::regex("[.!?]", REGEX_FLAGS)),
  std::make_pair(WHITESPACE, std::regex("\\s+", REGEX_FLAGS)),
  std::make_pair(WORD, std::regex("[^\\s\\*/]+", REGEX_FLAGS)),
  std::make_pair(STAR, std::regex("\\*", REGEX_FLAGS)),
  std::make_pair(SLASH, std::regex("/", REGEX_FLAGS))
};

DocToken DocTokenizer::next() {
  DocToken token;
  auto iter = source.cbegin();
  auto end = source.cend();
  if (iter != end) {
    for (auto& [type, regex] : regexes) {
      std::match_results<TextLineCursor::const_iterator> match;
      if (std::regex_search(iter, end, match, regex,
          std::regex_constants::match_continuous)) {
        token.type = type;
        // token.value = std::string_view(iter, iter + match.length());
        token.value = source.substr(iter, match.length());
        // iter += match.length();
        source.advance(match.length());
        return token;
      }
    }
    error("unrecognized token");
  }
  return token;
}

DocToken DocTokenizer::consume(const int stop) {
  DocToken token = next();
  while (token.type && !(token.type & stop)) {
    token = next();
  }
  return token;
}
