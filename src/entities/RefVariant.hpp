#pragma once

#include <variant>
#include <type_traits>
#include <string_view>

#include "RootEntity.hpp"
#include "GroupEntity.hpp"
#include "TypeEntity.hpp"
#include "FunctionEntity.hpp"
#include "VariableEntity.hpp"
#include "EnumEntity.hpp"
#include "NamespaceEntity.hpp"
#include "TypedefEntity.hpp"
#include "ConceptEntity.hpp"
#include "OperatorEntity.hpp"
#include "MacroEntity.hpp"
#include "Entities.hpp"

template <template <typename...> typename Template> using EntityVariant = std::variant<
  Template<RootEntity>,
  Template<GroupEntity>,
  Template<TypeEntity>,
  Template<FunctionEntity>,
  Template<VariableEntity>,
  Template<EnumEntity>,
  Template<NamespaceEntity>,
  Template<TypedefEntity>,
  Template<ConceptEntity>,
  Template<OperatorEntity>,
  Template<MacroEntity>
>;

class RefVariant: public EntityVariant<FwdRef> {
public:
  using EntityVariant<FwdRef>::EntityVariant;

  constexpr const std::string_view get_docs() const noexcept {
    return std::visit([]<entity T>(const FwdRef<T>& e) { return e->get_docs(); }, *this);
  }

  // free helper: get entity name from a RefVariant instance
  constexpr friend std::string_view entity_name(const RefVariant& v) noexcept {
    return std::visit([](auto&& arg) {
      return entity_name<typename std::remove_cvref_t<decltype(arg)>::value_type>();
    }, v);
  }

  constexpr std::string_view get_entity_name() const noexcept {
    return std::visit([](auto&& arg) {
      using ref_t = std::remove_cvref_t<decltype(arg)>;
      using value_t = std::remove_cv_t<typename ref_t::value_type>;
      return entity_name<value_t>();
    }, *this);
  }

  constexpr std::string_view get_name() const noexcept {
    return std::visit([]<typename T>(const FwdRef<T>& e) -> std::string_view {
      return e->get_name();
    }, *this);
  }

  constexpr bool is_visible() const noexcept {
    return std::visit([]<typename T>(const FwdRef<T>& e) { return e->is_visible(); }, *this);
  }

  constexpr std::string_view get_group() const noexcept {
    return std::visit([]<typename T>(const
      FwdRef<T>& e) {
      return e->get_ingroup().view();
    }, *this);
  }

  template<entity T> constexpr bool is_type() const noexcept {
    return std::holds_alternative<FwdRef<T>>(*this);
  }

  constexpr void set_docs(Doc&& doc) noexcept {
    std::visit([&doc]<typename T>(FwdRef<T>& e) {
      e->set_doc(std::move(doc));
    }, *this);
  }
};

template<> struct std::formatter<RefVariant, char>: StatefulEntityFormatter {
  template<class FmtContext> FmtContext::iterator format(const RefVariant& refvar, FmtContext& ctx) const {
    const bool dbg = this->dbg;
    return std::visit([dbg, &ctx]<typename T>(const FwdRef<T>& ref) -> FmtContext::iterator {
      if (not dbg) {
        return std::format_to(ctx.out(), "{}", ref);
      }
      return std::format_to(ctx.out(), "{:?}", ref);
    }, refvar);
  }
};
