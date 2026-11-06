#pragma once

#include "TextLineCursor.hpp"
#include "entities/TextCoordinates.hpp"
#include <generated/cpp_query_enums.hpp>

#include <tree_sitter/api.h>

#include <cassert>
#include <cstdint>
#include <limits>
#include <memory.h>
#include <string_view>
#include <utility>


struct TextLineCoordinate {
  static constexpr uint32_t INVALID = std::numeric_limits<uint32_t>::max();

  uint32_t char_pos = INVALID;
  uint32_t line = INVALID;

  constexpr bool is_valid() const noexcept {
    return char_pos != INVALID;
  }

  constexpr void reset() noexcept {
    char_pos = INVALID;
    line = INVALID;
  }

  constexpr void from_start(const TSNode& node) noexcept {
    char_pos = ts_node_start_byte(node);
    line = ts_node_start_point(node).row;
  }

  constexpr void from_end(const TSNode& node) noexcept {
    char_pos = ts_node_end_byte(node);
    line = ts_node_end_point(node).row;
  }

  constexpr uint32_t dist_from(const TextLineCoordinate& t) const noexcept {
    assert(this->char_pos >= t.char_pos);
    return this->char_pos - t.char_pos;
  }

  constexpr void decr_by(const std::pair<uint32_t, uint32_t>& v) noexcept {
    char_pos -= v.first;
    line -= v.second;
  }
};

static constexpr std::pair<uint32_t, uint32_t> from_decl_end(const TextLineCursor& str, uint32_t start, uint32_t end) {
  uint32_t char_decr = 0, line_decr = 0;
  while(end > start && (
      std::isspace(str[end - 1])
      or str[end - 1] == '\\'
      or str[end - 1] == '='
      or str[end - 1] == ';'
    )
  ) {
    if (str[end - 1] == '\n') {
      line_decr++;
    }
    char_decr++;
    end--;

  }
  return std::make_pair(char_decr, line_decr);
}

static constexpr std::pair<uint32_t, uint32_t> from_decl_end(const TextLineCursor& str,
  const TextLineCoordinate& start, const TextLineCoordinate& end) {
  return from_decl_end(str, start.char_pos, end.char_pos);
}

struct FieldLineCoordinates {
  TextLineCoordinate start;
  TextLineCoordinate end;

  FieldLineCoordinates() = default;

  FieldLineCoordinates(const TSNode& node) {
    start.from_start(node);
    end.from_end(node);
  }

  constexpr bool is_valid() const noexcept {
    return start.is_valid();
  }

  constexpr void reset() noexcept {
    start.reset();
    end.reset();
  }

  constexpr uint32_t len() const noexcept {
    return end.dist_from(start);
  }

  constexpr std::string_view within(const std::string_view& v) const {
    return v.substr(start.char_pos, len());
  }

  constexpr std::string_view within(const TextLineCursor& v) const {
    return within(v.view());
  }

  constexpr TextLineCursor cursor_within(const TextLineCursor& v) const {
    return v.substr(start.char_pos, len());
  }
};

struct EntityFieldsMatch {
  QueryCppNodes entity_type;
  bool is_nested = false;
  FieldLineCoordinates decl;
  FieldLineCoordinates name;
  FieldLineCoordinates template_decl;
  FieldLineCoordinates body;

  constexpr EntityFieldsMatch() = default;
  constexpr EntityFieldsMatch(const EntityFieldsMatch&) = default;
  constexpr EntityFieldsMatch& operator=(const EntityFieldsMatch&) = default;

  constexpr void reset() noexcept {
    is_nested = false;
    decl.reset();
    name.reset();
    template_decl.reset();
    body.reset();
  }

  constexpr const TextLineCoordinate& get_start() const noexcept {
    return decl.start;
  }

  constexpr const TextLineCoordinate& get_end() const noexcept {
    return body.end;
  }

  constexpr TextCoordinates make_name_coords() const noexcept {
    return TextCoordinates(name.start.char_pos - get_start().char_pos, name.len());
  }

  constexpr bool is_valid() const noexcept {
    return decl.is_valid();
  }
};

enum class CommentType {
  NONDOC, // no comment
  MULTI_FORWARD, // block between /** and */
  MULTI_BACKWARD, // block between /**< or /*!< and */
  SINGLE_CONTINUE, // single-line comment ///
  SINGLE_CONTINUE_QT, // single-line comment //!
  SINGLE_BACKWARD_OPEN, // single-line backward comment ///<
  SINGLE_BACKWARD_OPEN_QT // single-line backward comment //!<
};

struct CommentMatch: FieldLineCoordinates {
  CommentType type = CommentType::NONDOC;

  constexpr CommentMatch() = default;
  constexpr CommentMatch(const CommentMatch&) = default;
  constexpr CommentMatch& operator=(const CommentMatch&) = default;

  constexpr void reset() noexcept {
    type = CommentType::NONDOC;
    FieldLineCoordinates::reset();
  }

  constexpr bool is_valid() const noexcept {
    return type != CommentType::NONDOC;
  }

  constexpr bool is_forward() const noexcept {
    return type == CommentType::MULTI_FORWARD
    or type == CommentType::SINGLE_CONTINUE
    or type == CommentType::SINGLE_CONTINUE_QT;
  }

  constexpr bool is_backward() const noexcept {
    return type == CommentType::MULTI_BACKWARD
    or type == CommentType::SINGLE_BACKWARD_OPEN
    or type == CommentType::SINGLE_BACKWARD_OPEN_QT;
  }

  constexpr void merge(CommentType _type, const FieldLineCoordinates& capture) {
    if (not is_valid()) {
      throw std::runtime_error("comment field is not initialized");
    } else {
      if (type != _type) {
        throw std::runtime_error(
          std::format("attempting to merge different comment types between lines {} and {}",
            static_cast<FieldLineCoordinates*>(this)->end.line + 1, capture.start.line + 1));
      }
      if (capture.start.line != static_cast<FieldLineCoordinates*>(this)->end.line + 1) {
        throw std::runtime_error(
          std::format("attempting to merge non-consecutive comments between lines {} and {}",
            static_cast<FieldLineCoordinates*>(this)->end.line + 1, capture.start.line + 1));
      }
      // just extend
      static_cast<FieldLineCoordinates*>(this)->end = capture.end;

    }
  }

  constexpr void init_from(CommentType _type, const FieldLineCoordinates& capture) {
    type = _type;
    *static_cast<FieldLineCoordinates*>(this) = capture;
  }
};

constexpr bool backward_comment_overlaps(const CommentMatch& cm, const EntityFieldsMatch& em) {
  auto lmin = std::min(em.body.end.line, cm.start.line);
  auto lmax = std::max(em.body.end.line, cm.start.line);
  // also allow backward comments that are one line after the end of the entity
  return (lmax - lmin) <= 1;
}

constexpr bool is_new_comment(const CommentMatch& existing_comment, CommentType type, const FieldLineCoordinates& capture) {
  return
    // comments are of different format
    existing_comment.type != type
    // or there is gap between comments
    or capture.start.line > existing_comment.end.line + 1
    // the new comment is an entire block
    or type == CommentType::MULTI_FORWARD
    or type == CommentType::MULTI_BACKWARD;
}

template<typename PtrT, auto Deleter> struct _uptr_holder:
  std::unique_ptr<PtrT, decltype(Deleter)> {
  constexpr _uptr_holder(PtrT* p):
  std::unique_ptr<PtrT, decltype(Deleter)>(p, Deleter) {}

  constexpr _uptr_holder<PtrT, Deleter>(_uptr_holder<PtrT, Deleter>&&) = default;

  constexpr _uptr_holder<PtrT, Deleter>& operator=(_uptr_holder<PtrT, Deleter>&&) = default;

  constexpr _uptr_holder<PtrT, Deleter>& operator=(PtrT* p) {
    this->reset(p);
    return *this;
  }
};

using query_cursor_uptr = _uptr_holder<TSQueryCursor, ts_query_cursor_delete>;

using parse_tree_uptr = _uptr_holder<TSTree, ts_tree_delete>;

struct tree_cursor_uptr: private TSTreeCursor {
  constexpr tree_cursor_uptr(TSTreeCursor&& c): TSTreeCursor{std::move(c)} {}

  constexpr TSTreeCursor* operator&() { return this; }

  constexpr ~tree_cursor_uptr() {
    ts_tree_cursor_delete(this);
  }
};
