#pragma once

#include "Documentable.hpp"
#include "EntityFormatter.hpp"
#include "FileEntity.hpp"
#include "Entities.hpp"
#include "FwdRef.hpp"
#include "TextCoordinates.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <format>
#include <utility>

class _DeclStorage {
  protected:
    std::string decl;

    constexpr _DeclStorage(std::string_view _decl): decl(_decl) {}

    constexpr _DeclStorage(std::string&& _decl): decl(std::move(_decl)) {}

  public:
    constexpr std::string_view get_decl() const noexcept { return decl; }
};

class CodeEntity: public _DeclStorage, public Documentable {
protected:
  std::string_view path;
  uint32_t start_line;
  uint32_t end_line;

public:
  constexpr CodeEntity(std::string_view decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, const Doc& docs, bool visible):
    _DeclStorage{decl},
    Documentable(this->get_decl().substr(name_coords.offset(), name_coords.len()), docs, visible),
    path{file->get_path()},
    start_line{start_line},
    end_line{end_line}
    {}

  constexpr CodeEntity(std::string_view decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, Doc&& docs, bool visible):
  _DeclStorage{decl},
  Documentable(this->get_decl().substr(name_coords.offset(), name_coords.len()), std::move(docs), visible),
  path{file->get_path()},
  start_line{start_line},
  end_line{end_line}
  {}

  constexpr CodeEntity(std::string&& decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, const Doc& docs, bool visible):
  _DeclStorage{std::move(decl)},
  Documentable(this->get_decl().substr(name_coords.offset(), name_coords.len()), docs, visible),
  path{file->get_path()},
  start_line{start_line},
  end_line{end_line}
  {}

  constexpr CodeEntity(std::string&& decl, const TextCoordinates& name_coords, const FwdRef<FileEntity>& file, uint32_t start_line, uint32_t end_line, Doc&& docs, bool visible):
  _DeclStorage{std::move(decl)},
  Documentable(this->get_decl().substr(name_coords.offset(), name_coords.len()), std::move(docs), visible),
  path{file->get_path()},
  start_line{start_line},
  end_line{end_line}
  {}

public:
  std::string_view get_path() const noexcept { return path; }
  uint32_t get_start_line() const noexcept { return start_line; }
  uint32_t get_end_line() const noexcept { return end_line; }
};

// formatter for CodeEntity
template<entity T> requires (std::derived_from<T, CodeEntity>)
struct std::formatter<FwdRef<T>, char>: StatefulEntityFormatter {
  template<class FmtContext> FmtContext::iterator format(const FwdRef<T>& e, FmtContext& ctx) const {
    auto it = std::format_to(ctx.out(), "entity name \"{}\"", e->get_name());
    if (dbg) {
      it = std::format_to(it, ", from file \"{}\", lines {}-{}, declaration '''{}'''",
        e->get_path(), e->get_start_line() + 1, e->get_end_line() + 1, e->get_decl());
    }
    return it;
  }
};

// formatter for all other entities
template <entity T> requires (not std::derived_from<T, CodeEntity>)
struct std::formatter<FwdRef<T>, char>: StatefulEntityFormatter {
  template<class FmtContext> FmtContext::iterator format(const FwdRef<T>& e, FmtContext& ctx) const {
    auto it = std::format_to(ctx.out(), "{}", entity_name<T>());
    if (not e->get_name().empty()) {
      it = std::format_to(it, " '{}'", e->get_name());
    }
    return it;
  }
};
