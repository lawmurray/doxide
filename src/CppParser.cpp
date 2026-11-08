#include "CppParser.hpp"

#include "CppParserUtils.hpp"
#include "Doc.hpp"
#include "Log.hpp"
#include "TextLineCursor.hpp"
#include "CppQueries.hpp"
#include "Regex.hpp"
#include "StyleOption.hpp"
#include "Utils.hpp"
#include <doc_parser/StyleParsers.hpp>
#include <entities/EntityRegistry.hpp>
#include <entities/FileEntity.hpp>
#include <entities/RefVariant.hpp>
#include <entities/TextCoordinates.hpp>
#include <generated/cpp_query_enums.hpp>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <format>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <ranges>
#include <regex>
#include <stdexcept>
#include <string.h>
#include <tuple>
#include <unordered_map>
#include <utility>

class RootEntity;
class VariableEntity;

/**
 * Tree-sitter CUDA language handle.
 *
 * @ingroup developer
 */
extern "C" const TSLanguage* tree_sitter_cuda();

CppParser::CppParser(EntityRegistry& _reg, StyleOption style) :
    registry(_reg),
    doc_parser(make_doc_parser(style)),
    parser(nullptr),
    query(nullptr),
    query_exclude(nullptr),
    query_include(nullptr) {
  uint32_t error_offset;
  TSQueryError error_type;

  /* parser */
  parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_cuda());

  /* queries */
  query = ts_query_new(tree_sitter_cuda(), QUERY_CPP.data(),
      uint32_t(QUERY_CPP.length()), &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from = QUERY_CPP.substr(error_offset,
        std::min(std::size_t(40), QUERY_CPP.length() - error_offset));
    error("invalid query starting '" << from << "'...");
  }

  query_exclude = ts_query_new(tree_sitter_cuda(), QUERY_CPP_EXCLUDE.data(),
      uint32_t(QUERY_CPP_EXCLUDE.length()), &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from = QUERY_CPP_EXCLUDE.substr(error_offset,
        std::min(std::size_t(40), QUERY_CPP_EXCLUDE.length() - error_offset));
    error("invalid query starting '" << from << "'...");
  }

  query_include = ts_query_new(tree_sitter_cuda(), QUERY_CPP_INCLUDE.data(),
      uint32_t(QUERY_CPP_INCLUDE.length()), &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from = QUERY_CPP_INCLUDE.substr(error_offset,
        std::min(std::size_t(40), QUERY_CPP_INCLUDE.length() - error_offset));
    error("invalid query starting '" << from << "'...");
  }
}

CppParser::~CppParser() {
  ts_query_delete(query);
  ts_query_delete(query_exclude);
  ts_query_delete(query_include);
  ts_parser_delete(parser);
}

static constexpr bool cannot_contain_subentities(const RefVariant& ref) {
  return ref.is_type<VariableEntity>();
}

std::stack<CppParser::StackEntry>::const_reference CppParser::pop_entities_downto_parent(uint32_t start, [[maybe_unused]] uint32_t end) {
  while(entities_stack.size() > 0 and
    (entities_stack.top().end < start
      or cannot_contain_subentities(entities_stack.top().ref)
    )
  ) {
    entities_stack.pop();
  }
  if (entities_stack.size() == 0) {
    throw std::runtime_error("no more elements in stack");
  }
  return entities_stack.top();
}

void CppParser::pop_all() {
  while(entities_stack.size() > 0) {
    entities_stack.pop();
  }
}

std::stack<CppParser::StackEntry>::reference CppParser::push(RefVariant ref, uint32_t start, uint32_t end, bool child_nogroup) {
  return entities_stack.emplace(ref, start, end, child_nogroup);
}


// from https://www.doxygen.nl/manual/docblocks.html
static const std::regex comment_regex(
  R"(^\s*)" // beginning of string, than any whitespace (ignore)
  R"((?:)"  // start of non-capturing group
  R"((\/\*[\*|!])(?!<))" // (1) opening comment /** or /*!
  R"(|)"
  R"((\/\/\/)(?!<))" // (2) line continuation comment /// (without <)
  R"(|)"
  R"((\/\/!)(?!<))" // (3) Qt line continuation comment //! (without <)
  R"(|)"
  R"((\/\*[\*|!]<))" // (4) backward opening comment /**< or /*!<
  R"(|)"
  R"((\/\/\/<))" // (5) backward-looking comment ///<
  R"(|)"
  R"((\/\/!<))" // (6) Qt backward-looking comment //!<
  R"())", // end of non-capturing group
  REGEX_FLAGS);

static CommentType get_comment_type(const std::string_view& text) {
  // TODO: avoid heap allocations with CTRE
  std::cmatch match;
  if (std::regex_search(text.begin(), text.end(), match, comment_regex)) {
    if (match[1].matched) {
      return CommentType::MULTI_FORWARD;
    }
    else if (match[2].matched) {
      return CommentType::SINGLE_CONTINUE;
    }
    else if (match[3].matched) {
      return CommentType::SINGLE_CONTINUE_QT;
    }
    else if (match[4].matched) {
      return CommentType::MULTI_BACKWARD;
    }
    else if (match[5].matched) {
      return CommentType::SINGLE_BACKWARD_OPEN;
    }
    else if (match[6].matched) {
      return CommentType::SINGLE_BACKWARD_OPEN_QT;
    }
  }
  return CommentType::NONDOC;
}

enum class MatchAction {
  NOTHING,
  FLUSHED_DOCS,
  CAPTURED_ENTITY,
};


struct ParserState {
  const FwdRef<FileEntity>& file;
  EntityRegistry::EntityFileInserter& inserter;
};



void CppParser::check_group(Doc& docs, const ParserState& ps) const {
  if (not docs.ingroup.empty() and not registry.get_group(docs.ingroup.view())) {
    warn("file " << ps.file->get_path() << " line " << docs.ingroup.get_line_number() + 1 <<
        ": unrecognized group '" << docs.ingroup.view() <<
        "', groups must be defined in config file, ignoring @ingroup");
    docs.drop_group();
  }
}

constexpr RefVariant make_entity(
  const EntityFieldsMatch &match_state,
  const RefVariant& parent,
  const TSQuery* query,
  ParserState& ps,
  Doc& doc
) {
  // auto& ms = match_state;
  std::string_view content = ps.file->get_content();
  // std::string_view entity_decl = content.substr(ms.decl_field.start, ms.decl_field.len());
  std::string_view entity_decl = match_state.decl.within(content);
  const bool visible = not doc.docs.empty();
  std::optional<RefVariant> result_opt;
  QueryCppNodes id = match_state.entity_type;
  uint32_t start_line = match_state.get_start().line;
  uint32_t end_line = match_state.get_end().line;
  std::string_view template_decl = match_state.template_decl.is_valid() ?
    match_state.template_decl.within(content) : "";
  TextCoordinates name_coords = match_state.make_name_coords();

  switch (id) {
    case QueryCppNodes::FUNCTION: {
        auto ref = ps.inserter.add_function(parent, template_decl, entity_decl,
          name_coords, ps.file, start_line, end_line, std::move(doc), visible);
        result_opt.emplace(ref);
        break;
    }
    case QueryCppNodes::TYPE: {
        auto ref = ps.inserter.add_type(parent, template_decl, entity_decl,
          name_coords, ps.file, start_line, end_line, std::move(doc), visible);
        result_opt.emplace(ref);
        break;
    }
    case QueryCppNodes::VARIABLE: {
      auto ref = ps.inserter.add_variable(parent, template_decl, entity_decl,
        name_coords, ps.file, start_line, end_line, std::move(doc), visible);
      result_opt.emplace(ref);
      break;
    }
    case QueryCppNodes::CONCEPT: {
      auto ref = ps.inserter.add_concept(parent, template_decl, entity_decl,
        name_coords, ps.file, start_line, end_line, std::move(doc), visible);
      result_opt.emplace(ref);
      break;
    }
    case QueryCppNodes::OPERATOR: {
      auto ref = ps.inserter.add_operator(parent, template_decl, entity_decl,
        name_coords, ps.file, start_line, end_line, std::move(doc), visible);
      result_opt.emplace(ref);
      break;
    }
    case QueryCppNodes::ENUMERATOR: {
      auto ref = ps.inserter.add_enum(parent, entity_decl,
        name_coords, ps.file, start_line, end_line, std::move(doc), visible);
      result_opt.emplace(ref);
      break;
    }
    case QueryCppNodes::MACRO: {
      auto ref = ps.inserter.add_macro(parent, entity_decl,
        name_coords, ps.file, start_line, end_line, std::move(doc), visible);
      result_opt.emplace(ref);
      break;
    }
    case QueryCppNodes::TYPEDEF: {
      auto ref = ps.inserter.add_typedef(parent, template_decl, entity_decl,
        name_coords, ps.file, start_line, end_line, std::move(doc), visible);
      result_opt.emplace(ref);
      break;
    }
    default: {
      uint32_t len = 0;
      const char* name = ts_query_capture_name_for_id(query, static_cast<uint32_t>(id), &len);
      throw std::runtime_error(std::format("Error: unhandeld entity of type {}", name));
      break;
    }
  }
  return *result_opt;
}

static constexpr std::string_view NAMESPACE_SEPARATOR{"::"};

auto make_namespace_fields(
  const EntityFieldsMatch &match_state,
  ParserState& ps,
  const Doc& doc,
  const RefVariant& parent
) {
  std::string_view content = ps.file->get_content();
  std::string_view name = match_state.name.within(content);

  std::string full_name;
  if (parent.is_type<NamespaceEntity>()) {
    std::string_view ns_fn = std::get<FwdRef<NamespaceEntity>>(parent)->get_full_name();
    full_name.reserve(ns_fn.size() + 2 + name.size());
    full_name.append(ns_fn);
    full_name.append(NAMESPACE_SEPARATOR);
  } else {
    full_name.reserve(name.size());
  }
  return std::make_tuple(
    not doc.docs.empty(),
    match_state.make_name_coords(),
    match_state.decl.within(content),
    match_state.get_start().line,
    match_state.get_end().line,
    std::move(full_name)
  );
}

RefVariant CppParser::make_nested_namespace(
  const EntityFieldsMatch &match_state,
  const RefVariant& parent,
  ParserState& ps,
  Doc&& doc
) {
  auto [final_visible, name_coords, decl_view, start_line, end_line, full_name_start] =
    make_namespace_fields(match_state, ps, doc, parent);
  std::string_view ns_name = decl_view.substr(name_coords.offset(), name_coords.len());
  Doc empty_doc;
  auto parts = ns_name | std::views::split(NAMESPACE_SEPARATOR);
  std::optional<RefVariant> previous_opt = parent;
  std::size_t current_name_offset = name_coords.offset();
  for (auto it = parts.begin(); it != parts.end(); ++it) {
    std::size_t name_len = std::ranges::distance(*it);
    TextCoordinates name_coords{current_name_offset, name_len};
    std::string_view decl{decl_view.substr(0, current_name_offset + name_len)};
    full_name_start.append(name_coords.within(decl_view));
    current_name_offset += name_len + NAMESPACE_SEPARATOR.size(); // account for "::" separator
    const bool is_last = std::next(it) == parts.end();
    if (auto ns = registry.get_namespace(full_name_start);
      not is_last and ns.has_value()) {
      previous_opt = ns.value();
      // the namespace already exists: do not create it
    } else {
      Doc& d = not is_last? empty_doc : doc;
      const bool visible = is_last and final_visible;
      FwdRef<NamespaceEntity> child = ps.inserter.add_namespace(*previous_opt, decl, name_coords, ps.file,
        start_line, end_line, std::move(d), visible, full_name_start);
      std::format_to(std::ostream_iterator<char>(std::cout), ">> nested namespace \"{}\" created under {}: {:?}\n",
        full_name_start, *previous_opt, child);
      previous_opt = child;
    }
    if (not is_last) {
      full_name_start.append(NAMESPACE_SEPARATOR);
    }
  }
  return previous_opt.value();
}


FwdRef<NamespaceEntity> CppParser::make_single_namespace(
  const EntityFieldsMatch &match_state,
  const RefVariant& parent,
  ParserState& ps,
  Doc&& doc
) {
  auto [final_visible, name_coords, decl_view, start_line, end_line, full_name_start] =
    make_namespace_fields(match_state, ps, doc, parent);
  full_name_start.append(match_state.name.within(ps.file->get_content()));
  auto child = ps.inserter.add_namespace(parent, decl_view, name_coords,
    ps.file, start_line, end_line, std::move(doc), final_visible, std::move(full_name_start));
  std::format_to(std::ostream_iterator<char>(std::cout), ">> namespace \"{}\" created under {}: {:?}\n",
    child->get_full_name(), parent, child);
  return child;
}

void CppParser::register_entity(ParserState& parser_state, const EntityFieldsMatch& entity_match, Doc&& docs) {
  uint32_t start = entity_match.get_start().char_pos;
  uint32_t end = entity_match.get_end().char_pos;
  std::stack<CppParser::StackEntry>::const_reference stack_top = pop_entities_downto_parent(start, end);
  RefVariant parent = stack_top.ref;
  const bool child_has_no_group = stack_top.child_nogroup;
  if (not docs.ingroup.empty()) {
    if (child_has_no_group) {
      docs.drop_group();
    } else {
      auto group = registry.get_group(docs.ingroup);
      if (not group) {
        warn("file " << parser_state.file->get_name() << " line " << docs.ingroup.get_line_number() + 1
          << ": invalid @ingroup \"" << docs.ingroup << "\", ignoring");
        docs.drop_group();
      } else {
        parent = *group;
      }
    }
  }
  std::optional<RefVariant> child;
  if (entity_match.entity_type != QueryCppNodes::NAMESPACE) {
    child = make_entity(entity_match, parent, query, parser_state, docs);
    std::format_to(std::ostream_iterator<char>(std::cout), ">> {} \"{}\" created under {}: {:?}\n",
      child->get_entity_name(), child->get_name(), parent, *child);
  } else if (not entity_match.is_nested) {
    child = make_single_namespace(entity_match, parent, parser_state, std::move(docs));
  } else {
    child = make_nested_namespace(entity_match, parent, parser_state, std::move(docs));
  }
  // push child on top of the stack, based on the start and end of the entity
  const bool child_nogroup = child_has_no_group
   or entity_match.entity_type == QueryCppNodes::NAMESPACE
   or entity_match.entity_type == QueryCppNodes::TYPE;
  push(child.value(), start, end, child_nogroup);
}

void CppParser::flush_entity(ParserState& parser_state,
  EntityFieldsMatch& entity_match
) {
  if (not entity_match.is_valid()) {
    throw std::logic_error("the entity match must be valid");
  }
  Doc docs;
  register_entity(parser_state, entity_match, std::move(docs));
  entity_match.reset();
}

void CppParser::flush_entity_with_comment(
  ParserState& parser_state,
  EntityFieldsMatch& entity_match,
  CommentMatch& comment_match,
  const TextLineCursor& file_content,
  bool expect_backward
) {
  if (not entity_match.is_valid()) {
    throw std::logic_error("the entity match must be valid");
  }
  Doc docs;
  const bool is_valid_backward = expect_backward
    and comment_match.is_backward()
    and backward_comment_overlaps(comment_match, entity_match);
  const bool is_valid_forward = not expect_backward
    and comment_match.is_forward()
    and comment_match.end.line <= entity_match.get_start().line;
  if (is_valid_backward or is_valid_forward) {
    TextLineCursor comment_text = comment_match.cursor_within(file_content);
    docs = doc_parser->parse(comment_text);
    comment_match.reset();
  }
  register_entity(parser_state, entity_match, std::move(docs));
  entity_match.reset();
}

std::optional<FwdRef<FileEntity>> CppParser::parse(
  const std::filesystem::path& filename,
  const std::unordered_map<std::string, std::string>& defines,
  bool has_coverage
) {
  std::string content = preprocess(filename, defines);
  parse_tree_uptr tree(ts_parser_parse_string(parser, NULL, content.data(), static_cast<uint32_t>(content.size())));
  if (not tree) {
    /* something went very wrong */
    warn("cannot parse " << filename << ", skipping");
    return {};
  } else {
    /* report on any remaining parse errors */
    report(filename, content, tree.get());
  }
  TSNode node = ts_tree_root_node(tree.get());
  FwdRef<FileEntity> file = registry.add_file(filename, std::move(content), ts_node_end_point(node).row);
  parse_entities(file, node);
  if (has_coverage) {
    parse_line_count(file, tree.get());
  }
  return file;
}

void CppParser::parse_entities(
  const FwdRef<FileEntity>& file_ref,
  TSNode& root_node
) {
  FwdRef<RootEntity> root_ref = registry.get_root_ref();
  EntityRegistry::EntityFileInserter inserter = registry.get_inserter(file_ref);

  TSNode node{root_node};
  push(root_ref, 0, ts_node_end_byte(node), false);

  const TextLineCursor file_content(file_ref->get_content());
  query_cursor_uptr cursor(ts_query_cursor_new());
  ts_query_cursor_exec(cursor.get(), query, node);
  TSQueryMatch match;
  ParserState parser_state = {file_ref, inserter};

  EntityFieldsMatch entity_match;
  CommentMatch comment_match;
  EntityFieldsMatch previous_entity_match;
  CommentMatch previous_comment_match;

  while (ts_query_cursor_next_match(cursor.get(), &match)) {
    MatchAction action{MatchAction::NOTHING};
    bool matching_template = false;

    for (uint16_t i = 0; i < match.capture_count; ++i) {
      node = match.captures[i].node;
      FieldLineCoordinates capture(node);

      // {
      //   uint32_t len = 0;
      //   const char* name = ts_query_capture_name_for_id(query, match.captures[i].index, &len);
      //   std::cout << "---- matched " << name << "\n>>>" << capture.within(file_content) << "<<<" <<std::endl;
      // }

      QueryCppNodes node_id = static_cast<QueryCppNodes>(match.captures[i].index);
      switch (node_id) {
        case QueryCppNodes::DOCS: {
          CommentType type = get_comment_type(capture.within(file_content));
          if (type == CommentType::NONDOC) {
            // tree-sitter-cpp cannot distinguish between "documentation" comments
            // like `/** */` and "standard" ones like `//` or `/* */`; the latter
            // ones are categorized by the regex as `NONDOC` and must be ignored
            // manually here; the {previous_}comment_match variables only
            // store "documentation" comments
            break;
          }
          // const bool must_flush = is_new_comment(comment_match, type, capture);
          if (not comment_match.is_valid()) {
            // store new comment
            comment_match.init_from(type, capture);
          } else if (is_new_comment(comment_match, type, capture)
            and comment_match.is_valid()) {
            // remember comment_match and signal that we flushed
            previous_comment_match = comment_match;
            action = MatchAction::FLUSHED_DOCS;
            // forget comment_match and store new one
            comment_match.init_from(type, capture);
          } else {
            comment_match.merge(type, capture);
          }
          // if (comment_match.is_valid()) {
          //   std::cout << "docs is \n>>>>\n" << comment_match.within(file_content) << "\n<<<<\n" << std::endl;
          // }
          break;
        }
        case QueryCppNodes::VALUE: {
          entity_match.decl.end = capture.start;
          auto diff = from_decl_end(file_content, entity_match.decl.start, capture.start);
          entity_match.decl.end.decr_by(diff);
          if (entity_match.entity_type == QueryCppNodes::MACRO) {
            // tree-sitter-cpp recognizes the macro body as a value, so we use
            // its start to delimit the end of the declaration, in order to
            // include also the macro arguments (if any); we then move the end
            // back to the arguments parenthesis `)` through the spaces
            entity_match.body = capture;
          }
          break;
        }
        case QueryCppNodes::NAME:
        case QueryCppNodes::NESTED_NAME: {
          entity_match.name = capture;
          // the body starts where the name ends
          entity_match.body.start = capture.end;
          entity_match.is_nested = node_id == QueryCppNodes::NESTED_NAME;
          break;
        }
        case QueryCppNodes::BODY: {
          if (matching_template) {
            // done with template matching
            matching_template = false;
            // the template declaration ends where the body starts
            entity_match.template_decl.end = capture.start;
            auto diff = from_decl_end(file_content, entity_match.template_decl.start, capture.start);
            entity_match.template_decl.end.decr_by(diff);
          } else {
            // the declaration ends where the body starts
            entity_match.decl.end = capture.start;
            // and we should remove whitespaces, equal (=) and so on
            auto diff = from_decl_end(file_content, entity_match.decl.start, capture.start);
            entity_match.decl.end.decr_by(diff);
          }
          entity_match.body.end = capture.end;
          break;
        }
        case QueryCppNodes::CONCEPT:
        case QueryCppNodes::OPERATOR:
        case QueryCppNodes::ENUMERATOR:
        case QueryCppNodes::MACRO:
        case QueryCppNodes::TYPEDEF:
        case QueryCppNodes::VARIABLE:
        case QueryCppNodes::FUNCTION:
        case QueryCppNodes::TYPE: {
          action = MatchAction::CAPTURED_ENTITY;
          entity_match.entity_type = node_id;
          entity_match.decl = capture;
          // move declaration back from spaces, semi-colons, equals (=) and so on
          auto diff = from_decl_end(file_content, capture.start, capture.end);
          entity_match.decl.end.decr_by(diff);
          entity_match.body.end = capture.end;
          break;
        }
        case QueryCppNodes::TEMPLATE: {
          entity_match.template_decl = capture;
          entity_match.decl = capture;
          matching_template = true;
          break;
        }
        case QueryCppNodes::NAMESPACE: {
          action = MatchAction::CAPTURED_ENTITY;
          entity_match.entity_type = node_id;
          entity_match.decl = capture;
          break;
        }
      }
    }

    switch (action) {
      case MatchAction::NOTHING: {
        break;
      }
      case MatchAction::FLUSHED_DOCS: {
        // assert that previous_comment_match is valid and that previous_comment_match->is_backward()
        if (previous_entity_match.is_valid()) {
          // the entity was found before the comment: the comment must be backward
          // std::cout << "flush1" << std::endl;
          flush_entity_with_comment(parser_state, previous_entity_match, previous_comment_match, file_content, true);
        } else {
          previous_comment_match.reset();
        }
        // INVARIANT: here, previous_comment_match is not valid anymore:
        // it was either associated backward to a pre-existing entity
        // or it was reset, because it is superseded by the newer comment
        break;
      }
      case MatchAction::CAPTURED_ENTITY: {
        // assert that entity_match is valid;

        if (previous_entity_match.is_valid()) {
          // consume previous_entity_match in any case, and consume comment_match if backward
          // std::cout << "flush2" << std::endl;
          flush_entity_with_comment(parser_state, previous_entity_match, comment_match, file_content, true);
        }
        if (comment_match.is_forward()) {
          // consume comment_match by associating it to entity_match
          // std::cout << "flush3" << std::endl;
          flush_entity_with_comment(parser_state, entity_match, comment_match, file_content, false);
          // if there was a previous comment, it was spurious
          previous_comment_match.reset();
        } else if (comment_match.is_backward()) {
          // reset comment_match, as it is backward and no previous entity exists
          comment_match.reset();
        }
        if (entity_match.is_valid()) {
          // if we haven't consumed entity_match, save it for when we match a comment
          previous_entity_match = entity_match;
          entity_match.reset();
        }
        // INVARIANTS here
        // 1. entity_match is not valid anymore, either consumed
        // with comment_match or stored into previous_entity_match;
        // 2. comment_match is not valid anymore, given to previous_entity_match (if backward),
        // to entity_match (if forward) or discarded (if backward but no previous_entity_match exists)
        break;
      }
    }
  }

  if (previous_entity_match.is_valid()) {
    // there is a previous entity: attempt to give comment to it
    // std::cout << "flush4" << std::endl;
    flush_entity_with_comment(parser_state, previous_entity_match, comment_match, file_content, true);
  }
  // pop remaining entries to apply the child's visibility and empty the stack
  pop_all();
}

void CppParser::parse_line_count(FwdRef<FileEntity>& file, TSTree* tree) const {
  // sorted map of END, START, for optimized lookup of lower_bound (result >= query)
  std::map<uint32_t,uint32_t> excluded;
  bool constexpr_context = false;
  query_cursor_uptr cursor(ts_query_cursor_new());
  TSNode node = ts_tree_root_node(tree);
  ts_query_cursor_exec(cursor.get(), query_exclude, node);
  TSQueryMatch match;
  std::string_view file_content = file->get_content();
  while (ts_query_cursor_next_match(cursor.get(), &match)) {
    for (uint16_t i = 0; i < match.capture_count; ++i) {
      node = match.captures[i].node;
      uint32_t id = match.captures[i].index;
      uint32_t start = ts_node_start_byte(node);
      uint32_t end = ts_node_end_byte(node);
      switch (static_cast<QueryCppExcludeNodes>(id)) {
        case QueryCppExcludeNodes::EXCLUDE: {
          /* exclude any expressions in this region for line data */
          excluded.emplace(end, start);
          break;
        }
        case QueryCppExcludeNodes::THEN_EXCLUDE: {
          /* to be excluded if the last constexpr check was positive */
          if (constexpr_context) {
            excluded.emplace(end, start);
            constexpr_context = false;
          }
          break;
        }
        case QueryCppExcludeNodes::IF_CONSTEXPR: {
          static const std::regex regex_if_constexpr("^(?:if\\s+)?constexpr", REGEX_FLAGS);
          /* check if this is `constexpr`, which is not reflected in the
          * parse tree and requires a string comparison */
          std::string_view stmt = file_content.substr(start, end - start);
          constexpr_context = std::regex_search(stmt.cbegin(), stmt.cend(), regex_if_constexpr);
          break;
        }
      }
    }
  }

  cursor = query_cursor_uptr(ts_query_cursor_new());
  node = ts_tree_root_node(tree);
  ts_query_cursor_exec(cursor.get(), query_include, node);
  auto& line_counts = file->get_line_counts();
  while (ts_query_cursor_next_match(cursor.get(), &match)) {
    for (uint16_t i = 0; i < match.capture_count; ++i) {
      node = match.captures[i].node;
      uint32_t id = match.captures[i].index;
      uint32_t start = ts_node_start_byte(node);
      uint32_t end = ts_node_end_byte(node);
      switch (static_cast<QueryCppIncludeNodes>(id)) {
        case QueryCppIncludeNodes::EXECUTABLE: {
          /* executable code, update line data as long as the code is not
            * within an excluded region */
          auto it = excluded.lower_bound(end);
          bool exclude = it != excluded.end() and it->second <= start;
          if (not exclude) {
            uint32_t start_line = ts_node_start_point(node).row;
            uint32_t end_line = ts_node_end_point(node).row;
            for (uint32_t line = start_line; line <= end_line; ++line) {
              if (line_counts[line] < 0) {
                line_counts[line] = 0;
                file->get_included_lines()++;
              }
            }
          }
          break;
        }
      }
    }
  }
}

std::string CppParser::preprocess(const std::filesystem::path& filename,
    const std::unordered_map<std::string,std::string>& defines) {
  std::string in = gulp(filename);
  parse_tree_uptr tree(ts_parser_parse_string(parser, NULL, in.data(),
      uint32_t(in.size())));
  TSNode root = ts_tree_root_node(tree.get());
  TSNode node = root;
  tree_cursor_uptr cursor{ts_tree_cursor_new(root)};
  do {
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);
    TSPoint from = ts_node_start_point(node);
    bool nextNodeChosen = false;

    if (defines.contains(in.substr(k, l - k))) {
      /* replace preprocessor macro */
      const std::string& value = defines.at(in.substr(k, l - k));
      uint32_t old_size = uint32_t(in.size());
      in.replace(k, l - k, value);
      uint32_t new_size = uint32_t(in.size());
      TSPoint root_to = ts_node_end_point(root);

      /* update tree */
      TSInputEdit edit{k, old_size, new_size, from, root_to, root_to};
      ts_tree_edit(tree.get(), &edit);
      ts_parser_reset(parser);
      {
        parse_tree_uptr old_tree(std::move(tree));
        tree = ts_parser_parse_string(parser, old_tree.get(), in.data(),
            uint32_t(in.size()));
      }
      root = ts_tree_root_node(tree.get());

      /* restore cursor to same byte position as edit */
      ts_tree_cursor_reset(&cursor, root);
      while (ts_tree_cursor_goto_first_child_for_byte(&cursor, k) >= 0);

      /* next iteration from this position */
      nextNodeChosen = true;
    }

    /* next node */
    if (!nextNodeChosen) {
      constexpr std::string_view t1 = "preproc_def";
      constexpr std::string_view t2 = "preproc_function_def";
      if (strncmp(ts_node_type(node), t1.data(), t1.size()) != 0 &&
          strncmp(ts_node_type(node), t2.data(), t2.size()) != 0 &&
          ts_tree_cursor_goto_first_child(&cursor)) {
        // ^ do not recurse into preprocessor definitions, as we do not want
        //   to replace preprocessor macros there
      } else if (ts_tree_cursor_goto_next_sibling(&cursor)) {
        //
      } else while (ts_tree_cursor_goto_parent(&cursor) &&
          !ts_tree_cursor_goto_next_sibling(&cursor)) {
        //
      }
    }
    node = ts_tree_cursor_current_node(&cursor);
  } while (!ts_node_eq(node, root));

  ts_parser_reset(parser);
  return in;
}

void CppParser::report(const std::filesystem::path& filename,
    const std::string_view in, TSTree* tree) {
  TSNode root = ts_tree_root_node(tree);
  TSNode node = root;
  tree_cursor_uptr cursor = ts_tree_cursor_new(root);
  do {
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);
    TSPoint from = ts_node_start_point(node);
    if (ts_node_is_error(node)) {
      warn(filename << ':' << (from.row + 1) << ':' << from.column <<
          ": warning: parse error at '" <<
          in.substr(k, std::min(l - k, 40u)) <<
          "', but will continue");
    }
    /* next node */
    if (std::strcmp(ts_node_type(node), "preproc_def") != 0 &&
        std::strcmp(ts_node_type(node), "preproc_function_def") != 0 &&
        ts_tree_cursor_goto_first_child(&cursor)) {
      // ^ do not recurse into preprocessor definitions
    } else if (ts_tree_cursor_goto_next_sibling(&cursor)) {
      //
    } else while (ts_tree_cursor_goto_parent(&cursor) &&
        !ts_tree_cursor_goto_next_sibling(&cursor)) {
      //
    }
    node = ts_tree_cursor_current_node(&cursor);
  } while (!ts_node_eq(node, root));
}
