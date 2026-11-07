#pragma once


#include <entities/ContainerBases.hpp>
#include <entities/Entities.hpp>
#include <entities/GroupEntity.hpp>
#include <entities/NamespaceEntity.hpp>
#include <entities/RefVariant.hpp>
#include <entities/TypeEntity.hpp>
#include "entities/EntityRegistry.hpp"
#include "Sanitize.hpp"

#include <array>
#include <concepts>
#include <cstddef>
#include <format>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

template <typename T> class FwdRef;

template<entity T> struct entity_children_range_t {
  using entity_t = T;
  using range_t = EntityRegistry::children_view_t<T>;
  range_t view;
  std::ranges::iterator_t<range_t> iter;
  std::ranges::sentinel_t<range_t> end;

  constexpr entity_children_range_t(range_t&& r):
    view(std::move(r)), iter(std::ranges::begin(view)), end(std::ranges::end(view)) {}

  constexpr entity_children_range_t(const entity_children_range_t& other) = delete;

  constexpr entity_children_range_t<T>& operator=(const entity_children_range_t<T>&) = delete;

  constexpr entity_children_range_t(entity_children_range_t&& other) noexcept:
    view(std::move(other.view)), iter(std::ranges::begin(view)), end(std::ranges::end(view)) {}
};

using IterPackVariant = std::variant<
  entity_children_range_t<GroupEntity>,
  entity_children_range_t<TypeEntity>,
  entity_children_range_t<NamespaceEntity>
>;

template<entity T> decltype(auto) _in_place = std::in_place_type<entity_children_range_t<T>>;

template<entity T> constexpr IterPackVariant get_pack(std::size_t index, const FwdRef<T>& node, const EntityRegistry& registry) {
  constexpr std::string_view msg = "no entity of type '{}' is stored in an entity of type '{}'";
  switch (index) {
    case 0: {
      if constexpr (container_of<T, GroupEntity>) {
        return IterPackVariant{_in_place<GroupEntity>, registry.get_groups_view()};
      } else {
        throw std::logic_error(std::format(msg, entity_name<GroupEntity>(), entity_name<T>()));
      }
    }
    case 1: {
      if constexpr (container_of<T, TypeEntity>) {
        return IterPackVariant{_in_place<TypeEntity>, registry.get_type_children_for(node)};
      } else {
        throw std::logic_error(std::format(msg, entity_name<TypeEntity>(), entity_name<T>()));
      }
    }
    case 2: {
      if constexpr (container_of<T, NamespaceEntity>) {
        return IterPackVariant{_in_place<NamespaceEntity>, registry.get_namespace_children_for(node)};
      } else {
        throw std::logic_error(std::format(msg, entity_name<NamespaceEntity>(), entity_name<T>()));
      }
    }
    default: {
      throw std::logic_error(std::format("unreachable index {} for an entity of type {}", index, entity_name<T>()));
    }
  };
}

template<entity T> struct IterPackVariantIterator {
  constexpr static std::size_t N = std::variant_size_v<IterPackVariant>;
  std::array<std::size_t, N> next_index;
  std::size_t first_index;

  constexpr std::size_t get_next(std::size_t index) const noexcept {
    return next_index[index];
  }

  constexpr bool has_next(std::size_t index) const noexcept {
    return get_next(index) > index;
  }
};

template<entity T> consteval IterPackVariantIterator<T> get_valid_iter_pack_iter() {
  constexpr std::size_t N = std::variant_size_v<IterPackVariant>;
  constexpr std::array<bool, N> parent_of = []<std::size_t... I>(std::index_sequence<I...>) constexpr {
    return std::array<bool, N>{
      (container_of<T, typename std::variant_alternative_t<I, IterPackVariant>::entity_t>)...
    };
  }(std::make_index_sequence<N>{});
  std::array<std::size_t, N> res;
  std::size_t last_valid = 0;
  for (auto it = res.rbegin(); it != res.rend(); ++it) {
    std::size_t index = std::distance(res.begin(), it.base()) - 1;
    *it = last_valid != 0 ? last_valid : index;
    last_valid = parent_of[index] ? index : last_valid;
  }
  return IterPackVariantIterator<T>{res, last_valid};
}

template<entity T> static constexpr IterPackVariantIterator PACK_ITER = get_valid_iter_pack_iter<T>();

constexpr static IterPackVariant get_first_ref_variant(const RefVariant& ref, const EntityRegistry& registry) {
  return std::visit([&registry]<entity T>(const FwdRef<T>& _ref) -> IterPackVariant {
    return get_pack(PACK_ITER<T>.first_index, _ref, registry);
  }, ref);
}

// This visitor stores the navigation state of a *single* node. It tracks which
// list we are looking at, and holds a type-safe iterator inside that active list.
class EntityIterStack {
private:
  template<typename F> constexpr decltype(auto) visit_storage_typed(F&& fun) {
    return std::visit([&fun]<entity T>(entity_children_range_t<T>& _storage)
      -> std::invoke_result_t<F, entity_children_range_t<T>&> {
      static_assert(std::invocable<F, entity_children_range_t<T>&>, "fun is not invocable");
      return fun(_storage);
    }, range);
  }

  template<typename F> constexpr decltype(auto) visit_storage_typed(F&& fun) const {
    return std::visit([&fun]<entity T>(const entity_children_range_t<T>& _storage)
      -> std::invoke_result_t<F, const entity_children_range_t<T>&> {
      static_assert(std::invocable<F, entity_children_range_t<T>&>, "fun is not invocable");
      return fun(_storage);
    }, range);
  }

  // initializes the variant with the precise type-safe begin iterator
  constexpr void step_to_valid_iter(const EntityRegistry& registry) {
    std::visit([this, &registry]<entity T>(const FwdRef<T>& n) {
      // move to the first set of iterators storing actual elements
      while(not has_next() and PACK_ITER<T>.has_next(range.index())) {
        std::visit([this]<entity U>(entity_children_range_t<U>&& storage) {
          // because it's not copy assignable, access the internal state and move it
          range.template emplace<entity_children_range_t<U>>(std::move(storage));
        }, get_pack(PACK_ITER<T>.get_next(range.index()), n, registry)
        );
      }
    }, node);
  }

public:
  RefVariant node;
  std::string sanitized_name;
  // a variant capable of holding the active iterator type across any list configuration
  IterPackVariant range;

  constexpr EntityIterStack(const RefVariant& n, const std::string& sn, const EntityRegistry& registry) :
    node(n), sanitized_name(sn), range(get_first_ref_variant(node, registry)) {
    if (not has_next()) {
      step_to_valid_iter(registry);
    }
  }

  constexpr EntityIterStack(EntityIterStack&&) = default;

  // verifies if the current iterator has elements
  constexpr bool has_next() const {
    auto fun = []<entity T>(const entity_children_range_t<T>& range) -> bool {
      return range.iter != range.end;
    };
    return visit_storage_typed(fun);
  }

  // Returns the active child pointer safely packed inside a polymorphic NodeVariant wrapper
  constexpr const RefVariant get_next() const {
    auto fun = []<entity T>(const entity_children_range_t<T>& range) -> const RefVariant {
      return RefVariant(*range.iter);
    };
    return visit_storage_typed(fun);
  }

  // Manually steps the active type-safe iterator forward by one element
  constexpr void advance_iterator(const EntityRegistry& registry) {
    auto fun = []<entity T>(entity_children_range_t<T>& range) -> bool {
      if (range.iter != range.end) {
        ++(range.iter);
        return range.iter != range.end;
      }
      return false;
    };
    if (visit_storage_typed(fun)) {
      return;
    };
    step_to_valid_iter(registry);
  }
};

struct EntityStackController {
  using stack_value = EntityIterStack;
  using yield_value = std::tuple<const RefVariant, const std::string&, const std::filesystem::path&>;

  const EntityRegistry& registry;
  const std::filesystem::path& root_path;
  std::filesystem::path output_path = root_path;

  stack_value make_init() {
    return stack_value{RefVariant(registry.get_root_ref()), "", registry};
  }

  yield_value make_yield_pre(const stack_value& top) {
    return std::tie(top.node, top.sanitized_name, output_path);
  }

  bool has_next(const stack_value& top) const {
    return top.has_next();
  }

  stack_value make_next(stack_value& top) {
    const RefVariant next_child = top.get_next();
    // 4. Advance the active iterator so we don't process this exact child again
    top.advance_iterator(registry);
    // current_frame.advance_iterator();
    if (not top.sanitized_name.empty()) {
      output_path /= top.sanitized_name;
    }
    // 5. Push the new child to the top of the stack to process it on the next loop iteration (DFS)
    return stack_value{next_child, sanitize(next_child.get_name()), registry};
  }

  void after_pop(const stack_value& top) {
    if (not top.sanitized_name.empty()) {
      output_path = output_path.parent_path();
    }
  }
};
