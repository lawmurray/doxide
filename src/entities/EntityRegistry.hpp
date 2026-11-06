#pragma once

#include "ConceptEntity.hpp"
#include "Entities.hpp"
#include "EnumEntity.hpp"
#include "FileEntity.hpp"
#include "FunctionEntity.hpp"
#include "FwdList.hpp"
#include "GroupEntity.hpp"
#include "MacroEntity.hpp"
#include "NamespaceEntity.hpp"
#include "OperatorEntity.hpp"
#include "RefVariant.hpp"
#include "RootEntity.hpp"
#include "TypeEntity.hpp"
#include "VariableEntity.hpp"
#include "TypedefEntity.hpp"

#include <filesystem>
#include <format>
#include <iterator>
#include <list>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <utility>

class EntityRegistry {

  template<entity T> class entities_storage {
  public:
    using value_t = FwdRef<T>;
    using storage_t = std::list<std::vector<value_t>>;
    using sequence_ref = storage_t::reference;
    using sequence_iter = storage_t::iterator;
    using const_view_t = std::remove_cvref_t<decltype(std::declval<const storage_t&>() | std::views::join)>;

    // initialize empty
    constexpr entities_storage() = default;

    sequence_iter back_iter() {
      if (data.empty()) {
        _push_back_sequence();
      }
      return _back_iter();
    }

    sequence_iter back_new_iter() {
      if (data.empty() or not data.back().empty()) {
        _push_back_sequence();
      }
      return _back_iter();
    }

    const_view_t get_view() const {
      return data | std::views::join;
    }

    void erase_sequence(sequence_iter& it) {
      data.erase(it);
    }

    static inline const_view_t get_empty_view() {
      return EMPTY | std::views::join;
    }

    void clear() { data.clear(); }

  private:
    inline static const storage_t EMPTY;

    void _push_back_sequence() {
      data.push_back(std::vector<value_t>());
    }

    sequence_iter _back_iter() {
      return std::prev(data.end());
    }

    storage_t data;
  };

  template <template <typename> typename T> struct entity_content {
    T<TypeEntity> types;
    T<FunctionEntity> functions;
    T<VariableEntity> variables;
    T<EnumEntity> enums;
    T<NamespaceEntity> namespaces;
    T<TypedefEntity> typedefs;
    T<ConceptEntity> concepts;
    T<OperatorEntity> operators;
    T<MacroEntity> macros;

    template<entity U> auto& get_for() {
      if constexpr (std::same_as<U, TypeEntity>) {
        return types;
      } else if constexpr (std::same_as<U, FunctionEntity>) {
        return functions;
      } else if constexpr (std::same_as<U, VariableEntity>) {
        return variables;
      } else if constexpr (std::same_as<U, EnumEntity>) {
        return enums;
      } else if constexpr (std::same_as<U, NamespaceEntity>) {
        return namespaces;
      } else if constexpr (std::same_as<U, TypedefEntity>) {
        return typedefs;
      } else if constexpr (std::same_as<U, ConceptEntity>) {
        return concepts;
      } else if constexpr (std::same_as<U, OperatorEntity>) {
        return operators;
      } else /*if constexpr (std::same_as<ChildT, MacroEntity>)*/ {
        static_assert(std::same_as<U, MacroEntity>);
        return macros;
      }
    }

    template<entity U> const auto& get_for() const {
      if constexpr (std::same_as<U, TypeEntity>) {
        return types;
      } else if constexpr (std::same_as<U, FunctionEntity>) {
        return functions;
      } else if constexpr (std::same_as<U, VariableEntity>) {
        return variables;
      } else if constexpr (std::same_as<U, EnumEntity>) {
        return enums;
      } else if constexpr (std::same_as<U, NamespaceEntity>) {
        return namespaces;
      } else if constexpr (std::same_as<U, TypedefEntity>) {
        return typedefs;
      } else if constexpr (std::same_as<U, ConceptEntity>) {
        return concepts;
      } else if constexpr (std::same_as<U, OperatorEntity>) {
        return operators;
      } else /*if constexpr (std::same_as<ChildT, MacroEntity>)*/ {
        static_assert(std::same_as<U, MacroEntity>);
        return macros;
      }
    }
  };

  struct file_catalog_t {
    // sequence whose source in code_edges is the root and whose destinations were parsed in file
    template<typename T> using root_ref_t = std::optional<typename entities_storage<T>::sequence_iter>;
    // all parsed groups and the related destination sequence within code_edges
    template<typename T> using group_ref_t = std::unordered_map<FwdRef<GroupEntity>, typename entities_storage<T>::sequence_iter>;
    // all entities that are source in code_edges and are **not** root nor a group:
    // for each source s, the edge <s, [destinations from s]> can be removed entirely from code_edges
    template<typename T> using sources_ref_t = std::unordered_set<FwdHashable>;
    // all entities parsed in file (either source or destination):
    // they must be "vacuumed" from code_entities at the end of file removal
    template<typename T> using entities_ref_t = std::vector<FwdRef<T>>;

    entity_content<root_ref_t> root_sequence;
    entity_content<group_ref_t> groups_sequence;
    entity_content<sources_ref_t> sources;
    entity_content<entities_ref_t> destinations;

    file_catalog_t() = default;

    file_catalog_t(const file_catalog_t&) = delete;

    file_catalog_t& operator=(const file_catalog_t& other) = delete;
  };

public:

  template<entity T> using edges_map_t = std::unordered_map<FwdHashable, entities_storage<T>>;
  template<entity T> using children_view_t = entities_storage<T>::const_view_t;

private:

  template<entity ParentT, entity ChildT> requires container_of<ParentT, ChildT>
  children_view_t<ChildT> _get_children_view(const FwdRef<ParentT>& e) const {
    const edges_map_t<ChildT>& _edges = code_edges.get_for<ChildT>();
    auto it = _edges.find(e);
    return it != _edges.cend() ? it->second.get_view() : entities_storage<ChildT>::get_empty_view();
  }

  template<entity T> children_view_t<T> _get_children_for(const RefVariant& parent) const {
    return std::visit([this]<entity ParentT>(const FwdRef<ParentT>& p) -> children_view_t<T> {
      if constexpr (not container_of<ParentT, T>) {
        throw std::logic_error(std::format("Entities of type {} have no children of type {}",
          entity_name<ParentT>(), entity_name<T>()));
      } else {
        return this->_get_children_view<ParentT, T>(p);
      }
    }, parent);
  }

public:

  EntityRegistry() = delete;

  EntityRegistry(const std::string& title, const std::string& docs) {
    root.emplace_back(title, docs);
  }

  inline const FwdRef<RootEntity> get_root_ref() const noexcept {
    return root.front_ref();
  }

  inline FwdRef<RootEntity> get_root_ref() noexcept {
    return static_cast<FwdRef<RootEntity>>(root.begin());
  }

  inline FwdRef<GroupEntity> add_group(auto&&... args) {
    FwdRef<GroupEntity> res = static_cast<FwdRef<GroupEntity>>(group_entities.emplace_back(std::forward<decltype(args)>(args)...));
    std::string_view name = res->get_name();
    auto [it, newly_inserted] = name_group_map.emplace(name, res);
    if (not newly_inserted) {
      group_entities.erase(res);
      throw std::runtime_error(std::format("Group named \"{}\" already exists.", name));
    }
    group_edges.back_new_iter()->push_back(res);
    return res;
  }

  inline FwdRef<GroupEntity> add_group(FwdRef<GroupEntity> parent, auto&&... args) {
    FwdRef<GroupEntity> child = add_group(std::forward<decltype(args)>(args)...);
    group_hierarchy[parent].push_back(child);
    return child;
  }

  template<entity ChildT, entity ParentT> void remove_edges_sequence(const FwdRef<ParentT>& e,
    typename entities_storage<ChildT>::sequence_iter& sit) {
    auto& _edges = code_edges.get_for<ChildT>();
    auto it = _edges.find(e);
    if (it == _edges.end()) {
      throw std::runtime_error(std::format("no edge sequence \"{}\" of type \"{}\"", e->get_name(), entity_name(e)));
    }
    it->second.erase_sequence(sit);
  }

  template<entity T> void remove_file_root_edges(file_catalog_t& cat) {
    auto& rs = cat.root_sequence.get_for<T>();
    if (not rs) {
      return;
    }
    remove_edges_sequence<T>(*root.begin(), *rs);
    rs.reset();
  }

  template<entity T> void remove_file_group_edges(file_catalog_t& cat) {
    auto& groups = cat.groups_sequence.get_for<T>();
    for(auto& [group, group_sit]: groups) {
      remove_edges_sequence<T>(group, group_sit);
    }
    groups.clear();
  }

  template<entity T> void remove_file_sources(file_catalog_t& cat) {
    auto& _edges = code_edges.get_for<T>();
    auto& sources = cat.sources.get_for<T>();
    for(const auto& e: sources) {
      _edges.erase(e);
    }
    sources.clear();
  }

  template<entity T> void remove_file_catalog(file_catalog_t& cat) {
    auto& destinations = cat.destinations.get_for<T>();
    auto& entities = code_entities.get_for<T>();
    for(const auto& e: destinations) {
      if constexpr (std::same_as<T, NamespaceEntity>) {
        namespace_map.erase(e->get_full_name());
      }
      entities.erase(e);
    }
    destinations.clear();
  }

  class EntityFileInserter {
    template<entity ParentT, entity ChildT> requires container_of<ParentT, ChildT>
    FwdRef<ChildT> _add_child_to_parent(const FwdRef<ParentT>& parent, auto&&... args) {
      FwdRef<ChildT> result = static_cast<FwdRef<ChildT>>(
        registry.code_entities.get_for<ChildT>().emplace_back(std::forward<decltype(args)>(args)...));
      _add_to_parent(parent, result);
      return result;
    }

    template<entity ParentT, entity ChildT> requires container_of<ParentT, ChildT>
    void _add_to_parent(const FwdRef<ParentT>& parent, FwdRef<ChildT>& child) {
      if (not catalog) {
        catalog = std::ref(registry.catalog[file]);
      }
      EntityRegistry::file_catalog_t& cat = catalog.value().get();
      if constexpr (std::same_as<ParentT, RootEntity>) {
        auto& root_refs = cat.root_sequence.get_for<ChildT>();
        if (not root_refs) {
          root_refs = registry.code_edges.get_for<ChildT>()[parent].back_new_iter();
        }
        (*root_refs)->push_back(child);
      } else if constexpr (std::same_as<ParentT, GroupEntity>) {
        auto& group_refs = cat.groups_sequence.get_for<ChildT>();
        auto ref_it = group_refs.find(parent);
        if (ref_it == group_refs.end()) {
          auto iter = registry.code_edges.get_for<ChildT>()[parent].back_new_iter();
          auto [it, _] = group_refs.try_emplace(parent, iter);
          ref_it = it;
        }
        ref_it->second->push_back(child);
      } else {
        // get source per child type: the same parent can thus appear as father
        // of children of multiple types
        cat.sources.get_for<ChildT>().insert(parent);
        registry.code_edges.get_for<ChildT>()[parent].back_iter()->push_back(child);
      }
      if constexpr (std::same_as<ChildT, NamespaceEntity>) {
        registry.namespace_map.try_emplace(child->get_full_name(), child);
      }
      cat.destinations.get_for<ChildT>().push_back(child);
    }

    template<entity ChildT>
    FwdRef<ChildT> _add_child_to_parent_ref(const RefVariant& parent, auto&&... args) {
      FwdRef<ChildT> result = static_cast<FwdRef<ChildT>>(
        registry.code_entities.get_for<ChildT>().emplace_back(std::forward<decltype(args)>(args)...));
      std::visit([this, &result]<entity ParentT>(const FwdRef<ParentT>& p) {
        if constexpr (not container_of<ParentT, ChildT>) {
          std::string child_name{result->get_name()};
          registry.code_entities.get_for<ChildT>().erase(result);
          throw std::runtime_error(std::format("Cannot insert {} \"{}\" into {} \"{}\"",
            entity_name<ChildT>(), child_name, entity_name<ParentT>(), p->get_name()));
        } else {
          this->_add_to_parent(p, result);
        }
      }, parent);
      return result;
    }

  public:
    EntityFileInserter(std::string_view f, EntityRegistry& r): file(f), registry(r) {}

    EntityFileInserter(const EntityFileInserter& other) = delete;

    EntityFileInserter& operator=(const EntityFileInserter& other) = delete;

    template<entity ParentT> requires container_of<ParentT, TypeEntity>
    FwdRef<TypeEntity> add_type(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, TypeEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<TypeEntity> add_type(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<TypeEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    template<entity ParentT> requires container_of<ParentT, FunctionEntity>
    FwdRef<FunctionEntity> add_function(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, FunctionEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<FunctionEntity> add_function(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<FunctionEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    template<entity ParentT> requires container_of<ParentT, VariableEntity>
    FwdRef<VariableEntity> add_variable(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, VariableEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<VariableEntity> add_variable(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<VariableEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    template<entity ParentT> requires container_of<ParentT, EnumEntity>
    FwdRef<EnumEntity> add_enum(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, EnumEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<EnumEntity> add_enum(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<EnumEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    template<entity ParentT> requires container_of<ParentT, NamespaceEntity>
    FwdRef<NamespaceEntity> add_namespace(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, NamespaceEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<NamespaceEntity> add_namespace(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<NamespaceEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    template<entity ParentT> requires container_of<ParentT, TypedefEntity>
    FwdRef<TypedefEntity> add_typedef(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, TypedefEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<TypedefEntity> add_typedef(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<TypedefEntity>(parent, std::forward<decltype(args)>(args)...);
    }


    template<entity ParentT> requires container_of<ParentT, ConceptEntity>
    FwdRef<ConceptEntity> add_concept(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, ConceptEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<ConceptEntity> add_concept(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<ConceptEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    template<entity ParentT> requires container_of<ParentT, OperatorEntity>
    FwdRef<OperatorEntity> add_operator(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, OperatorEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<OperatorEntity> add_operator(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<OperatorEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    template<entity ParentT> requires container_of<ParentT, MacroEntity>
    FwdRef<MacroEntity> add_macro(const FwdRef<ParentT>& parent, auto&&... args) {
      return _add_child_to_parent<ParentT, MacroEntity>(parent, std::forward<decltype(args)>(args)...);
    }

    FwdRef<MacroEntity> add_macro(const RefVariant& parent, auto&&... args) {
      return _add_child_to_parent_ref<MacroEntity>(parent, std::forward<decltype(args)>(args)...);
    }

  private:
    std::string_view file;
    EntityRegistry& registry;
    std::optional<std::reference_wrapper<EntityRegistry::file_catalog_t>> catalog;
  };

  friend EntityFileInserter;

  EntityFileInserter get_inserter(const FwdRef<FileEntity>& file) {
    return EntityFileInserter(file->get_path(), *this);
  }

  FwdRef<FileEntity> add_file(const std::filesystem::path& path, auto&&... args) {
    if (path.empty() or not path.has_filename()) {
      throw std::runtime_error(std::format("empty file path or no filename"));
    }
    std::filesystem::path normalized = path.lexically_normal();
    FwdRef<FileEntity> fe = *file_entities.emplace_back(normalized, std::forward<decltype(args)>(args)...);
    auto [it, newly_inserted] = files_map.try_emplace(fe->get_path(), fe);
    if (not newly_inserted) {
      file_entities.erase(fe);
      throw std::runtime_error(std::format("file \"{}\" is already inserted", normalized.native()));
    }
    return fe;
  }

  inline std::optional<FwdRef<GroupEntity>> get_group(std::string_view name) const {
    auto it = name_group_map.find(name);
    if (it != name_group_map.cend()) {
      return {it->second};
    } else {
        return {};
    }
  }

  inline std::optional<FwdRef<NamespaceEntity>> get_namespace(std::string_view name) const {
    auto it = namespace_map.find(name);
    if (it != namespace_map.cend()) {
      return {it->second};
    } else {
        return {};
    }
  }

  children_view_t<GroupEntity> get_groups_view() const {
    return group_edges.get_view();
  }

  template<entity ParentT> requires container_of<ParentT, TypeEntity>
  children_view_t<TypeEntity> get_type_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, TypeEntity>(parent);
  }

  children_view_t<TypeEntity> get_type_children_for(const RefVariant& parent) const {
    return _get_children_for<TypeEntity>(parent);
  }

  template<entity ParentT> requires container_of<ParentT, TypeEntity>
  children_view_t<FunctionEntity> get_function_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, FunctionEntity>(parent);
  }

  children_view_t<FunctionEntity> get_function_children_for(const RefVariant& parent) const {
    return _get_children_for<FunctionEntity>(parent);
  }

  template<entity ParentT> requires container_of<ParentT, VariableEntity>
  children_view_t<VariableEntity> get_variable_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, VariableEntity>(parent);
  }

  children_view_t<VariableEntity> get_variable_children_for(const RefVariant& parent) const {
    return _get_children_for<VariableEntity>(parent);
  }

  template<entity ParentT> requires container_of<ParentT, EnumEntity>
  children_view_t<EnumEntity> get_enum_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, EnumEntity>(parent);
  }

  children_view_t<EnumEntity> get_enum_children_for(const RefVariant& parent) const {
    return _get_children_for<EnumEntity>(parent);
  }

  template<entity ParentT> requires container_of<ParentT, NamespaceEntity>
  children_view_t<NamespaceEntity> get_namespace_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, NamespaceEntity>(parent);
  }

  children_view_t<NamespaceEntity> get_namespace_children_for(const RefVariant& parent) const {
    return _get_children_for<NamespaceEntity>(parent);
  }

  template<entity ParentT> requires container_of<ParentT, TypedefEntity>
  children_view_t<TypedefEntity> get_typedef_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, TypedefEntity>(parent);
  }

  children_view_t<TypedefEntity> get_typedef_children_for(const RefVariant& parent) const {
    return _get_children_for<TypedefEntity>(parent);
  }

  template<entity ParentT> requires container_of<ParentT, ConceptEntity>
  children_view_t<ConceptEntity> get_concept_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, ConceptEntity>(parent);
  }

  children_view_t<ConceptEntity> get_concept_children_for(const RefVariant& parent) const {
    return _get_children_for<ConceptEntity>(parent);
  }

  template<entity ParentT> requires container_of<ParentT, OperatorEntity>
  children_view_t<OperatorEntity> get_operator_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, OperatorEntity>(parent);
  }

  children_view_t<OperatorEntity> get_operator_children_for(const RefVariant& parent) const {
    return _get_children_for<OperatorEntity>(parent);
  }

  template<entity ParentT> requires container_of<ParentT, MacroEntity>
  children_view_t<MacroEntity> get_macro_children_for(const FwdRef<ParentT>& parent) const {
    return _get_children_view<ParentT, MacroEntity>(parent);
  }

  children_view_t<MacroEntity> get_macro_children_for(const RefVariant& parent) const {
    return _get_children_for<MacroEntity>(parent);
  }

  void remove_file(const std::filesystem::path& file) {
    std::filesystem::path normalized = file.lexically_normal();
    std::string_view filename = normalized.native();
    auto it  = catalog.find(filename);
    if (it == catalog.end()) {
      return;
    }
    file_catalog_t& cat = it->second;
    auto dispatcher = [this]<entity T>(file_catalog_t& cat) {
      // remove <root, [destinations]> for file (if any)
      remove_file_root_edges<T>(cat);
      // for each group g, remove all <g, [destinations]> pairs for file (if any)
      remove_file_group_edges<T>(cat);
      // for each entity source, remove any <source, destinations> pair for file
      remove_file_sources<T>(cat);
      // deallocate all entities (both source and destination)
      remove_file_catalog<T>(cat);
    };

    dispatcher.operator()<TypeEntity>(cat);
    dispatcher.operator()<FunctionEntity>(cat);
    dispatcher.operator()<VariableEntity>(cat);
    dispatcher.operator()<EnumEntity>(cat);
    dispatcher.operator()<NamespaceEntity>(cat);
    dispatcher.operator()<TypedefEntity>(cat);
    dispatcher.operator()<ConceptEntity>(cat);
    dispatcher.operator()<OperatorEntity>(cat);
    dispatcher.operator()<MacroEntity>(cat);

    auto fit = files_map.find(filename);
    if (fit == files_map.end()) {
      throw std::logic_error(std::format("file \"{}\" not present", filename));
    }
    file_entities.erase(fit->second);
    files_map.erase(fit);
  }

  void clear() {
    auto type_dispatcher = []<template <typename> typename Container>(entity_content<Container>& storage, auto&& clearer) {
      clearer(storage.template get_for<TypeEntity>());
      clearer(storage.template get_for<FunctionEntity>());
      clearer(storage.template get_for<VariableEntity>());
      clearer(storage.template get_for<EnumEntity>());
      clearer(storage.template get_for<NamespaceEntity>());
      clearer(storage.template get_for<TypedefEntity>());
      clearer(storage.template get_for<ConceptEntity>());
      clearer(storage.template get_for<OperatorEntity>());
      clearer(storage.template get_for<MacroEntity>());
    };

    catalog.clear();
    namespace_map.clear();
    type_dispatcher(code_edges, [](auto&& em) { em.clear(); });
    group_edges.clear();
    type_dispatcher(code_entities, [](auto&& em) { em.clear(); });
    group_hierarchy.clear();
    name_group_map.clear();
    group_entities.clear();
    files_map.clear();
    file_entities.clear();
  }

private:

  FwdList<RootEntity> root;

  FwdList<FileEntity> file_entities;
  std::unordered_map<std::string_view, FwdRef<FileEntity>> files_map;

  // nodes
  FwdList<GroupEntity> group_entities;
  // map: group name -> group entity
  std::unordered_map<std::string_view, FwdRef<GroupEntity>> name_group_map;
  std::unordered_map<FwdRef<GroupEntity>, std::vector<FwdRef<GroupEntity>>> group_hierarchy;


  entity_content<FwdList> code_entities;

  // edges
  entities_storage<GroupEntity> group_edges;
  // map: source entity (FwdHashable) -> [destinations sequences]
  // the source entity can be **any** entity: root, group, ...
  entity_content<edges_map_t> code_edges;
  std::unordered_map<std::string_view, FwdRef<NamespaceEntity>> namespace_map;

  std::unordered_map<std::string_view, file_catalog_t> catalog;
};
