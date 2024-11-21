#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * C++ source parser.
 * 
 * @ingroup developer
 */
class Parser {
public:
  /**
   * Constructor.
   */
  Parser();

  /**
   * Destructor.
   */
  ~Parser();

  /**
   * Parse C++ sources.
   * 
   * @param filenames C++ source file names.
   * @param defines Macro definitions.
   */
  void parse(const std::unordered_set<std::string>& filenames);

  /**
   * Parse C++ source.
   * 
   * @param file C++ source file name.
   * @param defines Macro definitions.
   */
  void parse(const std::string& file);

  /**
   * Root entity.
   */
  Entity root;

  /**
   * Macro definitions.
   */
  std::unordered_map<std::string,std::string> defines;

private:
  /**
   * Push onto the stack.
   * 
   * @param entity Entity to push.
   * @param start Start byte of range.
   * @param end End byte of range.
   */
  void push(Entity&& entity, const uint32_t start, const uint32_t end);

  /**
   * Pop the stack down to the parent of an entity, according to its byte
   * range.
   * 
   * @param start Start byte of range.
   * @param end End byte of range.
   * 
   * @return The parent.
   * 
   * If both @p start and @p end are zero, this is interpreting as popping the
   * stack down to the root node and returning it.
   */
  Entity& pop(const uint32_t start = 0, const uint32_t end = 0);

  /**
   * Preprocess C++ source, replacing preprocessor macros as defined in the
   * config file and attempting to recover from any parse errors. This is
   * silent and does not report uncorrectable errors, these are reported
   * later.
   *
   * @param file C++ source file name.
   * @param defines Macro definitions.
   *
   * @return Preprocessed source.
   */
  std::string preprocess(const std::string& file);

  /**
   * Report errors after preprocessing.
   * 
   * @param file C++ source file name.
   * @param in Preprocessed source.
   * @param tree Parse tree for file.
   */
  void report(const std::string& file, const std::string& in, TSTree* tree);

  /**
   * Translate documentation comment.
   * 
   * @param comment Documentation comment.
   * @param entity Entity to document.
   */
  void translate(const std::string_view& comment, Entity& entity);

  /**
   * Stack of entities while parsing.
   */
  std::list<Entity> entities;

  /**
   * Stack of start bytes, corresponding to `entities`, while parsing.
   */
  std::list<uint32_t> starts;
  
  /**
   * Stack of end bytes, corresponding to `entities`, while parsing.
   */
  std::list<uint32_t> ends;

  /**
   * C++ parser.
   */
  TSParser* parser;

  /**
   * C++ entities query.
   */
  TSQuery* query;

  /**
   * C++ exclusions query.
   */
  TSQuery* query_exclude;

  /**
   * C++ inclusions query.
   */
  TSQuery* query_include;
};
