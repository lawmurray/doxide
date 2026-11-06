#pragma once

#include <Sink.hpp>
#include <entities/FwdRef.hpp>
#include <entities/Entities.hpp>

#include <span>
#include <string_view>

class DocRenderer {
public:

  virtual void render_groups(std::span<const FwdRef<GroupEntity>> groups) = 0;

  virtual void render_ns(std::span<const FwdRef<NamespaceEntity>> ns) = 0;

  virtual void print_coverage(std::string_view dir) = 0;

  virtual void render_enums(std::span<const FwdRef<EnumEntity>> ns) = 0;

  virtual void render_details(std::span<const FwdRef<TypedefEntity>> td) = 0;

  virtual void render_details(std::span<const FwdRef<ConceptEntity>> td) = 0;

  virtual void render_details(std::span<const FwdRef<MacroEntity>> td) = 0;

  virtual void render_details(std::span<const FwdRef<VariableEntity>> td) = 0;

  virtual void render_details(std::span<const FwdRef<OperatorEntity>> td) = 0;

  virtual void render_details(std::span<const FwdRef<FunctionEntity>> td) = 0;

  virtual ~DocRenderer() = default;
};

class DocRendererFactory {
public:

  virtual DocRenderer& get_renderer(sink_t& sink) = 0;

  virtual ~DocRendererFactory() = default;
};
