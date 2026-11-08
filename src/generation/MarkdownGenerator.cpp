#include "MarkdownGenerator.hpp"

#include <FileSystemDeleter.hpp>
#include <Sink.hpp>
#include <Utils.hpp>
#include <entities/ContainerBases.hpp>
#include <entities/Entities.hpp>
#include <entities/EntityRegistry.hpp>
#include <entities/FwdRef.hpp>
#include <entities/NamespaceEntity.hpp>
#include <entities/RefVariant.hpp>
#include <entities/TypeEntity.hpp>
#include "generation/DocRenderer.hpp"

#include "EntityIterStack.hpp"
#include "MdCommon.hpp"
#include "Sanitize.hpp"

#include <algorithm>
#include <fstream>
#include <ranges>
#include <string_view>
#include <variant>
#include <vector>
#include <type_traits>
#include <utility>

class RootEntity;

// checks that R is a range whose double de-ref value is an entity
template<typename R> concept entity_range = requires (R r) {
  requires std::ranges::range<R>; // it's a range
  *std::ranges::begin(r); // the iterator points to a reference type (like FwdRef)
  typename std::remove_cvref_t<decltype(*std::ranges::begin(r))>::value_type; // the ref-box must have a value_type typedef (like FwdRef)
  requires entity<std::remove_cvref_t<
    typename std::remove_cvref_t<decltype(*std::ranges::begin(r))>::value_type>
    >; // if you de-ref, it must provide an FwdRef whose value_type is an entity
};

template<entity_range R> using entity_range_value_t = std::remove_cvref_t<decltype(std::declval<std::ranges::range_value_t<R>>())>;

template<entity_range R> static auto get_visible_view(R&& entities) {
  return entities | std::views::filter([](const auto& el) { return el->is_visible(); });
}

template<typename R>
  requires (std::ranges::random_access_range<R> and entity_range<R>)
  static void sort_by_name(R&& entities) {
  std::ranges::sort(entities, []<entity T>(const FwdRef<T>& e1, const FwdRef<T>& e2) {
    std::string_view n1 = e1->get_name(), n2 = e2->get_name();
    if (n1 != n2) {
      return n1 < n2;
    }
    std::string_view p1 = e1->get_path(), p2 = e2->get_path();
    if (p1 == p2) {
      return e1->get_start_line() < e2->get_start_line();
    }
    return p1 < p2;
  });
}

template <typename T>
class assigner_closure {
    std::vector<T>& dest_vec;

public:
    explicit assigner_closure(std::vector<T>& vec) : dest_vec(vec) {}

    template <std::ranges::input_range R> friend void operator|(R&& range, assigner_closure closure) {
      if constexpr (std::ranges::sized_range<R>) {
          closure.dest_vec.reserve(std::ranges::size(range));
      }
      closure.dest_vec.assign(std::ranges::begin(range), std::ranges::end(range));
    }
};

template <typename T> auto assigner(std::vector<T>& vec) {
    return assigner_closure<T>(vec);
}

void print_md_table_file_link(sink_t& sink, entity_range auto&& entities, std::string_view caption) {
  if (std::ranges::empty(entities)) {
    return;
  }
  sink.println("## {}\n", caption);
  sink.println("| Name | Description |");
  sink.println("| ---- | ----------- |");
  for (const auto& e: entities) {
    sink.println("| [{}]({}/) | {} |", e->get_name(), sanitized(e->get_name()), liner(brief(e)));
  }
  sink.println();
}

void print_md_table(sink_t& sink, entity_range auto &&entities, std::string_view caption) {
  if (std::ranges::empty(entities)) {
    return;
  }
  sink.println("## {}\n", caption);
  sink.println("| Name | Description |");
  sink.println("| ---- | ----------- |");
  for (const auto& e: entities) {
    sink.println("| [{}](#{}) | {} |", e->get_name(), sanitized(e->get_name()), liner(brief(e)));
  }
  sink.println();
}

template<entity T> void MarkdownGenerator::render_entity(
  sink_t& sink, const EntityRegistry& registry, const FwdRef<T>& root,
  const std::optional<std::string_view>& coverage_directory) {

  constexpr bool sort_entities = std::same_as<T, NamespaceEntity> or std::same_as<T, GroupEntity>;

  sink.print(FRONTMATTER);
  sink.println("\n# {}\n", get_title(root));

  if constexpr (std::is_same_v<T, TypeEntity>) {
    sink.println("**`{}`**\n", liner(root->get_decl()));
  }
  sink.println("{}\n", root->get_docs());

  auto& doc_renderer = doc_render_factory->get_renderer(sink);

  if constexpr (groups_container<T>) {
    // anybody can see groups, but only Root is tagged as GroupsContainer
    get_visible_view(registry.get_groups_view()) | assigner(group_buffer);
    doc_renderer.render_groups(group_buffer);
  }

  if constexpr (namespaces_container<T>) {
    get_visible_view(registry.get_namespace_children_for(root)) | assigner(namespace_buffer);
    sort_by_name(namespace_buffer);
    doc_renderer.render_ns(namespace_buffer);
  }

  if constexpr (std::same_as<T, RootEntity>) {
    if (coverage_directory) {
      doc_renderer.print_coverage(*coverage_directory);
    }
  }

  if constexpr (types_container<T>) {
    get_visible_view(registry.get_type_children_for(root)) | assigner(type_buffer);
    if (sort_entities) {
      sort_by_name(type_buffer);
    }
    print_md_table_file_link(sink, type_buffer, "Types");
  }

  if constexpr (typedefs_container<T>) {
    get_visible_view(registry.get_typedef_children_for(root)) | assigner(typedef_buffer);
    if (sort_entities) { sort_by_name(typedef_buffer); }
    print_md_table(sink, typedef_buffer, "Type Aliases");
  }

  if constexpr (concepts_container<T>) {
    get_visible_view(registry.get_concept_children_for(root)) | assigner(concept_buffer);
    if (sort_entities) { sort_by_name(concept_buffer); }
    print_md_table(sink, concept_buffer, "Concepts");
  }

  if constexpr (macros_container<T>) {
    get_visible_view(registry.get_macro_children_for(root)) | assigner(macro_buffer);
    if (sort_entities) { sort_by_name(macro_buffer); }
    print_md_table(sink, macro_buffer, "Macros");
  }

  if constexpr (variables_container<T>) {
    get_visible_view(registry.get_variable_children_for(root)) | assigner(variable_buffer);
    if (sort_entities) { sort_by_name(variable_buffer); }
    print_md_table(sink, variable_buffer, "Variables");
  }

  if constexpr (operators_container<T>) {
    get_visible_view(registry.get_operator_children_for(root)) | assigner(operator_buffer);
    if (sort_entities) { sort_by_name(operator_buffer); }
    print_md_table(sink, operator_buffer, "Operators");
  }

  if constexpr (functions_container<T>) {
    get_visible_view(registry.get_function_children_for(root)) | assigner(function_buffer);
    if (sort_entities) { sort_by_name(function_buffer); }
    print_md_table(sink, function_buffer, "Functions");
  }

  if constexpr (enums_container<T>) {
    get_visible_view(registry.get_enum_children_for(root)) | assigner(enum_buffer);
    if (not std::ranges::empty(enum_buffer)) {
      doc_renderer.render_enums(enum_buffer);
    }
  }

  if constexpr (typedefs_container<T>) {
    if (not std::ranges::empty(typedef_buffer)) {
      if (not sort_entities) { sort_by_name(typedef_buffer); }
      doc_renderer.render_details(typedef_buffer);
    }
  }

  if constexpr (concepts_container<T>) {
    if (not std::ranges::empty(concept_buffer)) {
      if (not sort_entities) { sort_by_name(concept_buffer); }
      doc_renderer.render_details(concept_buffer);
    }
  }

  if constexpr (macros_container<T>) {
    if (not std::ranges::empty(macro_buffer)) {
      if (not sort_entities) { sort_by_name(macro_buffer); }
      doc_renderer.render_details(macro_buffer);
    }
  }

  if constexpr (variables_container<T>) {
    if (not std::ranges::empty(variable_buffer)) {
      if (not sort_entities) { sort_by_name(variable_buffer); }
      doc_renderer.render_details(variable_buffer);
    }
  }

  if constexpr (operators_container<T>) {
    if (not std::ranges::empty(operator_buffer)) {
      if (not sort_entities) { sort_by_name(operator_buffer); }
      doc_renderer.render_details(operator_buffer);
    }
  }

  if constexpr (functions_container<T>) {
    if (not std::ranges::empty(function_buffer)) {
      if (not sort_entities) { sort_by_name(function_buffer); }
      doc_renderer.render_details(function_buffer);
    }
  }

  group_buffer.clear();
  type_buffer.clear();
  function_buffer.clear();
  variable_buffer.clear();
  enum_buffer.clear();
  namespace_buffer.clear();
  typedef_buffer.clear();
  concept_buffer.clear();
  operator_buffer.clear();
  macro_buffer.clear();
}

void MarkdownGenerator::render_entity(const EntityRegistry& registry, const std::filesystem::path& output, const RefVariant& root, const std::string_view& sanitized_name, const std::optional<std::string_view>& coverage_directory) {
  constexpr std::string_view filename = "index.md";
  const std::string_view dirname = root.is_type<RootEntity>() ? "" : sanitized_name;
  sink_t csink;
  csink.println("\tdirname \"{}\", filename \"{}\", output \"{}\"", dirname, filename, output.native());
  std::filesystem::path file = output;
  if (dirname.size() > 0) {
    file /= dirname;
  }
  std::filesystem::create_directories(file);
  file /= filename;

  if (not can_write(file)) {
    return;
  }
  fscleaner.add_file(file);
  std::ofstream out(file);
  sink_t sink(out);

  std::visit([&]<typename T>(const FwdRef<T>& ref) {
    render_entity(sink, registry, ref, coverage_directory);
  }, root);
}

void MarkdownGenerator::generate(const EntityRegistry& registry,
  const std::optional<std::string_view>& coverage_directory) {
  sink_t sink;
  EntityStackController controller{registry, output};
  for (const auto& [v, sanitized_name, output_path]:
    dag_dfs_traverse(controller)) {
    sink.println("- getting entity: {} - \"{}\" - [sanitized: '{}'] on path {}",
      v.get_entity_name(), v.get_name(), sanitized_name, output_path.native());
    if (v.is_visible()) {
      render_entity(registry, output_path, v, sanitized_name, coverage_directory);
    }
  }
}
