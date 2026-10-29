#pragma once

#include "entities/FileEntity.hpp"
#include "entities/FwdRef.hpp"
#include "entities/RefVariant.hpp"

#include <cstdint>
#include <tree_sitter/api.h>
#include <filesystem>
#include <memory>
#include <optional>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>

class DocParser;
class EntityRegistry;
enum class StyleOption;

/**
 * C++ source parser.
 *
 * @ingroup developer
 */
class CppParser {
public:
  /**
   * Constructor.
   */
  CppParser(StyleOption style);

  /**
   * Destructor.
   */
  ~CppParser();

  /**
   * Parse C++ source.
   *
   * @param file C++ source file name.
   * @param style Markdown style.
   * @param defines Macro definitions.
   * @param[in,out] root Root entity.
   */
  std::optional<FwdRef<FileEntity>> parse(
    EntityRegistry& registry,
    const std::filesystem::path& filename,
    const std::unordered_map<std::string, std::string>& defines,
    bool has_coverage
  );

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
  std::string preprocess(const std::filesystem::path& file,
      const std::unordered_map<std::string,std::string>& defines);

  /**
   * Report errors after preprocessing.
   *
   * @param file C++ source file name.
   * @param in Preprocessed source.
   * @param tree Parse tree for file.
   */
  void report(const std::filesystem::path& file, const std::string_view in, TSTree* tree);

  struct StackEntry {
    RefVariant ref;
    uint32_t start;
    uint32_t end;
  };

  template<entity T> std::stack<StackEntry>::reference emplace_entity(FwdRef<T> ref, uint32_t start, uint32_t end) {
    return entities_stack.emplace(ref, start, end);
  }

  std::stack<StackEntry>::const_reference pop_entities_downto_parent(uint32_t start, uint32_t end);

  void pop_all();

  std::stack<StackEntry>::reference push(RefVariant ref, uint32_t start, uint32_t end);

  FwdRef<FileEntity> parse_entities(
    TSTree* tree,
    std::string&& content,
    EntityRegistry& registry,
    const std::filesystem::path& filename
  );

  void parse_line_count(FwdRef<FileEntity>&, TSTree*) const;

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

  std::unique_ptr<DocParser> doc_parser;

  std::stack<StackEntry> entities_stack;

};
