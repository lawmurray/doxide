#include "CppParser.hpp"
#include "Doc.hpp"

CppParser::CppParser() :
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
  query = ts_query_new(tree_sitter_cuda(), query_cpp,
      strlen(query_cpp), &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from(query_cpp + error_offset,
        std::min(size_t(40), strlen(query_cpp) - error_offset));
    error("invalid query starting '" << from << "'...");
  }

  query_exclude = ts_query_new(tree_sitter_cuda(), query_cpp_exclude,
      strlen(query_cpp_exclude), &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from(query_cpp_exclude + error_offset,
        std::min(size_t(40), strlen(query_cpp_exclude) - error_offset));
    error("invalid query starting '" << from << "'...");
  }

  query_include = ts_query_new(tree_sitter_cuda(), query_cpp_include,
      strlen(query_cpp_include), &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from(query_cpp_include + error_offset,
        std::min(size_t(40), strlen(query_cpp_include) - error_offset));
    error("invalid query starting '" << from << "'...");
  }
}

CppParser::~CppParser() {
  ts_query_delete(query);
  ts_query_delete(query_exclude);
  ts_query_delete(query_include);
  ts_parser_delete(parser);
}

void CppParser::parse(const std::unordered_set<std::string>& filenames) {
  for (const std::string& filename: filenames) {
    parse(filename);
  }
}

void CppParser::parse(const std::string& filename) {
  assert(entities.empty());
  assert(starts.empty());
  assert(ends.empty());

  /* entity to represent file */
  Entity file;
  file.name = std::filesystem::path(filename).filename().string();
  file.decl = preprocess(filename);
  file.path = filename;
  file.start_line = 0;
  file.end_line = 0;
  file.type = EntityType::FILE;
  file.visible = true;

  /* parse */
  TSTree* tree = ts_parser_parse_string(parser, NULL, file.decl.data(),
      file.decl.size());
  if (!tree) {
    /* something went very wrong */
    warn("cannot parse " << filename << ", skipping");
    return;
  } else {
    /* report on any remaining parse errors */
    report(filename, file.decl, tree);
  }

  /* query entity information */
  TSNode node = ts_tree_root_node(tree);

  file.start_line = 0;
  file.end_line = ts_node_end_point(node).row;
  file.line_counts.resize(file.end_line, -1);

  /* push root entity to stack */
  push(std::move(root), ts_node_start_byte(node), ts_node_end_byte(node));

  TSQueryCursor* cursor = ts_query_cursor_new();
  ts_query_cursor_exec(cursor, query, node);
  TSQueryMatch match;
  Entity entity;
  while (ts_query_cursor_next_match(cursor, &match)) {    
    uint32_t start = 0, middle = 0, end = 0;
    uint32_t start_line = -1, end_line = -1;
    for (uint16_t i = 0; i < match.capture_count; ++i) {
      node = match.captures[i].node;
      uint32_t id = match.captures[i].index; 
      uint32_t length = 0;
      const char* name = ts_query_capture_name_for_id(query, id, &length);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);

      if (strncmp(name, "docs", length) == 0) {
        Doc doc(file.decl.substr(k, l - k));
        Entity& e = doc.open.type == OPEN_BEFORE ? entity : entities.back();
        e.docs.append(doc.docs);
        e.hide = e.hide || doc.hide;
        if (!doc.ingroup.empty()) {
          e.ingroup = doc.ingroup;
        }
      } else if (strncmp(name, "nested_name", length) == 0) {
        assert(entity.type == EntityType::NAMESPACE);

        /* pop the stack down to parent */
        pop(start, end);

        /* nested namespace specifier, e.g. `namespace a::b::c`, split up the
         * name on `::`, push namespaces for the first n - 1 identifiers, and
         * assign the last as the name of this entity */
        static std::regex sep("\\s*::\\s*");
        
        /* load into a std::string and use std::sregex_token_iterator;
         * maintaining the std::string_view and using
         * std::cregex_token_iterator seems to have memory issues when mixed
         * with other local variables */
        std::string name = file.decl.substr(k, l - k);
        std::sregex_token_iterator ns_iter(name.begin(), name.end(), sep, -1);
        std::sregex_token_iterator ns_end;
        assert(ns_iter != ns_end);
        std::string prev = *ns_iter;  // lag one
        while (++ns_iter != ns_end) {
          /* create parent entity */
          Entity parent;
          parent.type = EntityType::NAMESPACE;
          parent.name = prev;

          push(std::move(parent), start, end);
          prev = *ns_iter;
        }
        entity.name = prev;
      } else if (strncmp(name, "name", length) == 0) {
        entity.name = file.decl.substr(k, l - k);
      } else if (strncmp(name, "body", length) == 0) {
        middle = ts_node_start_byte(node);
      } else if (strncmp(name, "value", length) == 0) {
        middle = ts_node_start_byte(node);
      } else {
        start = ts_node_start_byte(node);
        start_line = ts_node_start_point(node).row;
        end = ts_node_end_byte(node);
        end_line = ts_node_end_point(node).row;
        middle = end;

        if (strncmp(name, "namespace", length) == 0) {
          entity.type = EntityType::NAMESPACE;
        } else if (strncmp(name, "template", length) == 0) {
          entity.type = EntityType::TEMPLATE;
        } else if (strncmp(name, "type", length) == 0) {
          entity.type = EntityType::TYPE;
        } else if (strncmp(name, "concept", length) == 0) {
          entity.type = EntityType::CONCEPT;
        } else if (strncmp(name, "variable", length) == 0) {
          entity.type = EntityType::VARIABLE;
        } else if (strncmp(name, "function", length) == 0) {
          entity.type = EntityType::FUNCTION;
        } else if (strncmp(name, "operator", length) == 0) {
          entity.type = EntityType::OPERATOR;
        } else if (strncmp(name, "enumerator", length) == 0) {
          entity.type = EntityType::ENUMERATOR;
        } else if (strncmp(name, "macro", length) == 0) {
          entity.type = EntityType::MACRO;
        }
      }
    }
    if (entity.type != EntityType::ROOT) {
      /* workaround for entity declaration logic catching punctuation, e.g.
       * ending semicolon in declaration, the equals sign in a variable
       * declaration with initialization, or whitespace */
      while (middle > start && (file.decl[middle - 1] == ' ' ||
          file.decl[middle - 1] == '\t' ||
          file.decl[middle - 1] == '\n' ||
          file.decl[middle - 1] == '\r' ||
          file.decl[middle - 1] == '\\' ||
          file.decl[middle - 1] == '=' ||
          file.decl[middle - 1] == ';')) {
        --middle;
      }

      entity.decl = file.decl.substr(start, middle - start);
      entity.path = filename;
      entity.start_line = start_line;
      entity.end_line = end_line;
      entity.visible = !entity.docs.empty();

      /* the final node represents the whole entity, pop the stack until we
       * find its direct parent, as determined using nested byte ranges */
      Entity& parent = pop(start, end);

      /* override ingroup for entities that belong to a class or template, as
       * cannot be moved out */
      if (parent.type == EntityType::TYPE ||
          parent.type == EntityType::TEMPLATE) {
        entity.ingroup.clear();
      }

      /* push to stack */
      if (parent.type == EntityType::TEMPLATE) {
        /* merge this entity into the template */
        parent.merge(std::move(entity));
      } else {
        push(std::move(entity), start, end);
      }

      /* reset */
      entity.clear();
    }
  }

  /* finalize entity information */
  root = std::move(pop());

  entities.pop_back();
  starts.pop_back();
  ends.pop_back();

  ts_query_cursor_delete(cursor);

  /* determine excluded byte ranges for code coverage */
  std::list<std::pair<uint32_t,uint32_t>> excluded;
  bool constexpr_context = false;
  std::regex regex_if_constexpr("^(?:if\\s+)?constexpr");
  cursor = ts_query_cursor_new();
  node = ts_tree_root_node(tree);
  ts_query_cursor_exec(cursor, query_exclude, node);
  while (ts_query_cursor_next_match(cursor, &match)) {    
    for (uint16_t i = 0; i < match.capture_count; ++i) {
      node = match.captures[i].node;
      uint32_t id = match.captures[i].index; 
      uint32_t length = 0;
      uint32_t start = ts_node_start_byte(node);
      uint32_t end = ts_node_end_byte(node);
      const char* name = ts_query_capture_name_for_id(query_exclude, id,
          &length);
      if (strncmp(name, "exclude", length) == 0) {
        /* exclude any expressions in this region for line data */
        excluded.push_back(std::make_pair(start, end));
      } else if (strncmp(name, "if_constexpr", length) == 0) {
        /* check if this is `constexpr`, which is not reflected in the
         * parse tree and requires a string comparison */
        std::string stmt = file.decl.substr(start, end - start);
        constexpr_context = std::regex_search(stmt, regex_if_constexpr);
      } else if (strncmp(name, "then_exclude", length) == 0) {
        /* to be excluded if the last constexpr check was positive */
        if (constexpr_context) {
          excluded.push_back(std::make_pair(start, end));
          constexpr_context = false;
        }
      }
    }
  }
  ts_query_cursor_delete(cursor);

  /* determine included lines for code coverage */
  cursor = ts_query_cursor_new();
  node = ts_tree_root_node(tree);
  ts_query_cursor_exec(cursor, query_include, node);
  while (ts_query_cursor_next_match(cursor, &match)) {
    for (uint16_t i = 0; i < match.capture_count; ++i) {
      node = match.captures[i].node;
      uint32_t id = match.captures[i].index; 
      uint32_t length = 0;
      uint32_t start = ts_node_start_byte(node);
      uint32_t end = ts_node_end_byte(node);
      const char* name = ts_query_capture_name_for_id(query_include, id,
          &length);
      if (strncmp(name, "executable", length) == 0) {
        /* executable code, update line data as long as the code is not
         * within an excluded region */
        bool exclude = std::any_of(excluded.begin(), excluded.end(),
            [start,end](auto range) {
              return range.first <= start && end <= range.second;
            });
        if (!exclude) {
          uint32_t start_line = ts_node_start_point(node).row;
          uint32_t end_line = ts_node_end_point(node).row;
          for (uint32_t line = start_line; line <= end_line; ++line) {
            if (file.line_counts[line] < 0) {
              file.line_counts[line] = 0;
              ++file.lines_included;
            }
          }
        }
      }
    }
  }
  ts_query_cursor_delete(cursor);

  /* finish up */
  root.add(std::move(file));
  ts_tree_delete(tree);
  ts_parser_reset(parser);  

  assert(entities.empty());
  assert(starts.empty());
  assert(ends.empty());
}

void CppParser::push(Entity&& entity, const uint32_t start, const uint32_t end) {
  entities.push_back(std::move(entity));
  starts.push_back(start);
  ends.push_back(end);
}

Entity& CppParser::pop(const uint32_t start, const uint32_t end) {
  while (entities.size() > 1 &&
      (start < starts.back() || ends.back() < end ||
      (start == 0 && end == 0))) {
    Entity back = std::move(entities.back());
    entities.pop_back();
    if (back.ingroup.empty()) {
      entities.back().add(std::move(back));
    } else {
      entities.front().add(std::move(back));
    }
    starts.pop_back();
    ends.pop_back();
  }
  return entities.back();
}

std::string CppParser::preprocess(const std::string& filename) {
  /* regex to detect preprocessor macro names */
  static std::regex macro(R"([A-Z_][A-Z0-9_]{2,})");

  std::string in = gulp(filename);
  TSTree* tree = ts_parser_parse_string(parser, NULL, in.data(), in.size());
  TSNode root = ts_tree_root_node(tree);
  TSNode node = root;
  TSTreeCursor cursor = ts_tree_cursor_new(root);
  do {
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);
    TSPoint from = ts_node_start_point(node);
    TSPoint to = ts_node_end_point(node);
    bool nextNodeChosen = false;

    if (defines.contains(in.substr(k, l - k))) {
      /* replace preprocessor macro */
      std::string& value = defines.at(in.substr(k, l - k));
      uint32_t old_size = in.size();
      in.replace(k, l - k, value);
      uint32_t new_size = in.size();
      TSPoint root_to = ts_node_end_point(root);

      /* update tree */
      TSInputEdit edit{k, old_size, new_size, from, root_to, root_to};
      ts_tree_edit(tree, &edit);
      ts_parser_reset(parser);
      TSTree* old_tree = tree;
      tree = ts_parser_parse_string(parser, old_tree, in.data(), in.size());
      ts_tree_delete(old_tree);
      root = ts_tree_root_node(tree);

      /* restore cursor to same byte position as edit */
      ts_tree_cursor_reset(&cursor, root);
      while (ts_tree_cursor_goto_first_child_for_byte(&cursor, k) >= 0);

      /* next iteration from this position */
      nextNodeChosen = true;
    }

    /* next node */
    if (!nextNodeChosen) {
      if (strcmp(ts_node_type(node), "preproc_def") != 0 &&
          strcmp(ts_node_type(node), "preproc_function_def") != 0 &&
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

  ts_tree_cursor_delete(&cursor);
  ts_tree_delete(tree);
  ts_parser_reset(parser);
  return in;
}

void CppParser::report(const std::string& filename, const std::string& in,
    TSTree* tree) {
  TSNode root = ts_tree_root_node(tree);
  TSNode node = root;
  TSTreeCursor cursor = ts_tree_cursor_new(root);
  do {
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);
    TSPoint from = ts_node_start_point(node);
    if (ts_node_is_error(node)) {
      std::cerr << filename << ':' << (from.row + 1) << ':' << from.column <<
          ": warning: parse error at '" << in.substr(k, std::min(l - k, 40u)) <<
          "', but will continue" << std::endl;
    }

    /* next node */
    if (strcmp(ts_node_type(node), "preproc_def") != 0 &&
        strcmp(ts_node_type(node), "preproc_function_def") != 0 &&
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
  ts_tree_cursor_delete(&cursor);
}
