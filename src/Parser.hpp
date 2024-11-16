#pragma once

#include "doxide.hpp"
#include "Entity.hpp"
#include "File.hpp"

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
   * Source files.
   */
  std::list<File> files;

  /**
   * Macro definitions.
   */
  std::unordered_map<std::string,std::string> defines;

private:
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
   * C++ parser.
   */
  TSParser* parser;

  /**
   * C++ query.
   */
  TSQuery* query;
};
