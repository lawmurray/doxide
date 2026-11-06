#pragma once

#include <Doc.hpp>

#include <string>
#include <string_view>
#include <utility>

class Documentable {
protected:
  constexpr Documentable() = default;

  constexpr Documentable(std::string_view _name, const Doc& _doc, bool _visible): name(_name), doc(_doc), visible(_visible) {}

  constexpr Documentable(std::string_view _name, Doc&& _doc, bool _visible): name(_name), doc(std::move(_doc)), visible(_visible) {}

  constexpr Documentable(std::string_view _name, std::string_view _doc, bool _visible): name(_name), doc(_doc), visible(_visible) {}

  constexpr Documentable(const Documentable&) = default;

public:
  constexpr inline std::string_view get_name() const noexcept { return name; }

  constexpr inline const std::string_view get_docs() const noexcept { return doc.docs; }

  constexpr bool is_visible() const noexcept { return visible; }

  constexpr const TextLineCursor& get_ingroup() const noexcept { return doc.ingroup; }

  constexpr void set_doc(Doc&& newdoc) noexcept {
    doc = std::move(newdoc);
    visible = not doc.docs.empty();
  }

private:
  std::string_view name;
  Doc doc;
  bool visible = true;
};
