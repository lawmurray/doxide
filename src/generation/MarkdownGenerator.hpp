#pragma once

#include <entities/Entities.hpp>

#include "DocRenderer.hpp"
#include "DocRenderers.hpp"
#include "entities/FwdRef.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

class ConceptEntity;
class EntityRegistry;
class EnumEntity;
class FileSystemDeleter;
class FunctionEntity;
class GroupEntity;
class MacroEntity;
class NamespaceEntity;
class OperatorEntity;
class RefVariant;
class TypeEntity;
class TypedefEntity;
class VariableEntity;
class sink_t;
enum class StyleOption;

class MarkdownGenerator {
public:
  /**
    * Constructor.
    *
    * @param output Output directory.
    * @param style Markdown style.
    */
  MarkdownGenerator(const std::filesystem::path& output, FileSystemDeleter& _fscleaner, StyleOption style):
    output(output),
    fscleaner(_fscleaner),
    doc_render_factory(make_doc_renderer(style))
    {}

  /**
    * Generate documentation.
    *
    * @param root Root entity.
    * @param cov Include code coverage report?
    */
  void generate(const EntityRegistry& registry, const std::optional<std::string_view>& coverage_directory);

private:
  void render_entity(const EntityRegistry& registry, const std::filesystem::path& output, const RefVariant& root, const std::string_view& sanitized_name, const std::optional<std::string_view>& coverage_directory);

  template<entity T> void render_entity(
    sink_t& sink, const EntityRegistry& registry, const FwdRef<T>& root,
    const std::optional<std::string_view>& coverage_directory);

  /**
   * Output directory.
   */
  std::filesystem::path output;

  FileSystemDeleter& fscleaner;

  std::unique_ptr<DocRendererFactory> doc_render_factory;

  std::vector<FwdRef<GroupEntity>> group_buffer;

  std::vector<FwdRef<TypeEntity>> type_buffer;

  std::vector<FwdRef<FunctionEntity>> function_buffer;

  std::vector<FwdRef<VariableEntity>> variable_buffer;

  std::vector<FwdRef<EnumEntity>> enum_buffer;

  std::vector<FwdRef<NamespaceEntity>> namespace_buffer;

  std::vector<FwdRef<TypedefEntity>> typedef_buffer;

  std::vector<FwdRef<ConceptEntity>> concept_buffer;

  std::vector<FwdRef<OperatorEntity>> operator_buffer;

  std::vector<FwdRef<MacroEntity>> macro_buffer;
};
