#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * C++ source parser.
 */
class Parser {
public:
  /**
   * Constructor.
   */
  Parser(const std::unordered_map<std::string,std::string>& defines);

  /**
   * Destructor.
   */
  ~Parser();

  /**
   * Parse C++ source.
   * 
   * @param file C++ source file name.
   * @param global Global namespace.
   */
  void parse(const std::string& file, Entity& global);

private:
  /**
   * Preprocess C++ source.
   * 
   * @param file C++ source file name.
   * 
   * @return Preprocessed source.
   */
  std::string preprocess(const std::string& file);

  /**
   * Translate documentation comment.
   * 
   * @param comment Documentation comment.
   * @param entity Entity to document.
   */
  void translate(const std::string_view& comment, Entity& entity);

  /**
   * Defines.
   */
  std::unordered_map<std::string,std::string> defines;

  /**
   * C++ parser.
   */
  TSParser* parser;

  /**
   * C++ query.
   */
  TSQuery* query;
};
