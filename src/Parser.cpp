#include "Parser.hpp"
#include "Tokenizer.hpp"

Parser::Parser(const std::unordered_map<std::string,std::string>& defines) :
    defines(defines),
    parser(nullptr),
    query(nullptr) {
  uint32_t error_offset;
  TSQueryError error_type;

  /* parser */
  parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_cuda());

  /* query */
  query = ts_query_new(tree_sitter_cuda(), query_cpp, strlen(query_cpp),
      &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from(query_cpp + error_offset, 40);
    error("invalid query starting " << from << "...");
  }
}

Parser::~Parser() {
  ts_query_delete(query);
  ts_parser_delete(parser);
}

void Parser::parse(const std::string& file, Entity& global) {
  std::string in = preprocess(file);
  TSTree* tree = ts_parser_parse_string(parser, NULL, in.data(), in.size());
  if (!tree) {
    warn("cannot parse " << file << ", skipping");
  }
  report(file, in, tree);
  TSNode node = ts_tree_root_node(tree);

  /* initialize state */
  std::list<uint64_t> starts, ends;
  std::list<Entity> entities;

  starts.push_back(ts_node_start_byte(node));
  ends.push_back(ts_node_end_byte(node));
  entities.emplace_back(std::move(global));

  /* run query */
  TSQueryCursor* cursor = ts_query_cursor_new();
  ts_query_cursor_exec(cursor, query, node);
  TSQueryMatch match;
  Entity entity;
  while (ts_query_cursor_next_match(cursor, &match)) {    
    uint64_t start = 0, middle = 0, end = 0;
    uint64_t start_line = -1, middle_line = -1, end_line = -1;
    for (uint16_t i = 0; i < match.capture_count; ++i) {
      node = match.captures[i].node;
      uint32_t id = match.captures[i].index; 
      uint32_t length = 0;
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);
      const char* name = ts_query_capture_name_for_id(query, id, &length);

      if (strncmp(name, "docs", length) == 0) {
        std::string docs = in.substr(k, l - k);
        Tokenizer tokenizer(docs);
        Token token = tokenizer.next();
        if (token.type == OPEN_AFTER) {
          translate(docs, entities.back());
        } else if (token.type == OPEN_BEFORE) {
          translate(docs, entity);
        }
      } else if (strncmp(name, "name", length) == 0) {
        entity.name = in.substr(k, l - k);
      } else if (strncmp(name, "body", length) == 0) {
        middle = ts_node_start_byte(node);
        middle_line = ts_node_start_point(node).row;
      } else if (strncmp(name, "value", length) == 0) {
        middle = ts_node_start_byte(node);
        middle_line = ts_node_start_point(node).row;
      } else {
        start = ts_node_start_byte(node);
        start_line = ts_node_start_point(node).row;
        end = ts_node_end_byte(node);
        end_line = ts_node_end_point(node).row;
        middle = end;
        middle_line = end_line;

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
    if (entity.type != EntityType::NONE) {
      /* workaround for entity declaration logic catching punctuation, e.g.
       * ending semicolon in declaration, the equals sign in a variable
       * declaration with initialization, or whitespace */
      while (middle > start && (in[middle - 1] == ' ' ||
          in[middle - 1] == '\t' ||
          in[middle - 1] == '\n' ||
          in[middle - 1] == '\r' ||
          in[middle - 1] == '=' ||
          in[middle - 1] == ';')) {
        --middle;
      }

      /* entity declaration */
      entity.decl = in.substr(start, middle - start);

      /* entity location */
      entity.file = file;
      entity.start_line = start_line;
      entity.middle_line = middle_line;
      entity.end_line = end_line;

      /* the final node represents the whole entity, pop the stack until we
       * find its direct parent, as determined using nested byte ranges */
      while (start < starts.back() || ends.back() < end) {
        Entity back = std::move(entities.back());
        entities.pop_back();
        if (back.ingroup.empty()) {
          entities.back().add(back);
        } else {
          entities.front().add(back);
        }
        starts.pop_back();
        ends.pop_back();
      }

      /* override ingroup for entities that belong to a class or template, as
       * cannot be moved out */
      if (entities.back().type == EntityType::TYPE ||
          entities.back().type == EntityType::TEMPLATE) {
        entity.ingroup.clear();
      }

      /* push to stack */
      if (entities.back().type == EntityType::TEMPLATE) {
        /* merge this entity into the template */
        entities.back().merge(entity);
      } else {
        entities.emplace_back(std::move(entity));
        starts.push_back(start);
        ends.push_back(end);
      }

      /* reset */
      entity = Entity();
    }
  }

  /* finalize */
  while (entities.size() > 1) {
    Entity back = std::move(entities.back());
    entities.pop_back();
    if (back.ingroup.empty()) {
      entities.back().add(back);
    } else {
      entities.front().add(back);
    }
    starts.pop_back();
    ends.pop_back();
  }
  global = std::move(entities.back());

  entities.pop_back();
  starts.pop_back();
  ends.pop_back();

  assert(entities.empty());
  assert(starts.empty());
  assert(ends.empty());

  ts_query_cursor_delete(cursor);
  ts_tree_delete(tree);
  ts_parser_reset(parser);
}

std::string Parser::preprocess(const std::string& file) {
  /* regex to detect preprocessor macro names */
  static std::regex macro(R"([A-Z_][A-Z0-9_]{2,})");

  std::string in = gulp(file);
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
    } else if (ts_node_is_error(node)) {
      /* parse error: assuming that the syntax is actually valid, this is
       * usually caused by use of preprocessor macros, as the preprocessor is
       * not run */
      // std::cerr << file << ':' << (from.row + 1) << ':' << from.column <<
      //     ": warning: parse failed at '" << in.substr(k, l - k) << "'" <<
      //     std::endl;

      /* attempt recovery: step backward looking for a node that looks like
       * preprocessor macro use, and erase it */
      int back = 0;
      while (!std::regex_match(in.substr(k, l - k), macro) &&
          ts_tree_cursor_goto_previous_sibling(&cursor)) {
        node = ts_tree_cursor_current_node(&cursor);
        k = ts_node_start_byte(node);
        l = ts_node_end_byte(node);
        from = ts_node_start_point(node);
        to = ts_node_end_point(node);
        ++back;
      }
      if (std::regex_match(in.substr(k, l - k), macro)) {
        /* looks like a macro, erase it */
        // std::cerr << file << ':' << (from.row + 1) << ':' << from.column <<
        //     ": note: attempting recovery by erasing '" <<
        //     in.substr(k, l - k) << "'" << std::endl;

        /* overwrite with whitespace, rather than erasing entirely, to
         * preserve line and column numbers from user's perspective */
        uint32_t old_size = in.size();
        in.replace(k, l - k, l - k, ' ');
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
      } else {
        /* recovery failed, restore original position */
        // warn("recovery failed, continuing anyway");
        while (back--) {
          ts_tree_cursor_goto_next_sibling(&cursor);
        }
      }
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

void Parser::report(const std::string& file, const std::string& in,
    TSTree* tree) {
  TSNode root = ts_tree_root_node(tree);
  TSNode node = root;
  TSTreeCursor cursor = ts_tree_cursor_new(root);
  do {
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);
    TSPoint from = ts_node_start_point(node);
    TSPoint to = ts_node_end_point(node);

    if (ts_node_is_error(node)) {
      std::cerr << file << ':' << (from.row + 1) << ':' << from.column <<
          ": warning: parse failed at '" << in.substr(k, l - k) << "'" <<
          std::endl;
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

void Parser::translate(const std::string_view& comment, Entity& entity) {
  Tokenizer tokenizer(comment);
  Token token = tokenizer.next();
  token = tokenizer.next();
  bool first = true;  // first token
  if (!token.type) {
    /* empty end-of-line comment, consider end of paragraph */
    entity.indent = std::max(entity.indent - 4, 0);
  } else while (token.type) {
    if (token.type & COMMAND) {
      std::string_view command = token.substr(1);

      /* non-legacy commands */
      if (command == "param" ||
          command == "param[in]") {
        entity.docs.append("\n:material-location-enter: `");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`\n:   ");
        entity.indent = 4;
      } else if (command == "param[out]") {
        entity.docs.append("\n:material-location-exit: `");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`\n:   ");
        entity.indent = 4;
      } else if (command == "param[in,out]") {
        entity.docs.append("\n:material-location-enter::material-location-exit: `");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`\n:   ");
        entity.indent = 4;
      } else if (command == "tparam") {
        entity.docs.append("\n:material-code-tags: `");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`\n:   ");
        entity.indent = 4;
      } else if (command == "p") {
        entity.docs.append("`");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`");
      } else if (command == "return") {
        entity.docs.append("\n:material-keyboard-return: **Return**\n:   ");
      } else if (command == "pre") {
        entity.docs.append("\n:material-check-circle-outline: **Pre-condition**\n:   ");
      } else if (command == "post") {
        entity.docs.append("\n:material-check-circle-outline: **Post-condition**\n:   ");
      } else if (command == "throw") {
        entity.docs.append("\n:material-alert-circle-outline: **Throw**\n:   ");
      } else if (command == "see") {
        entity.docs.append("\n:material-eye-outline: **See**\n:   ");
      } else if (command == "anchor") {
        entity.docs.append("<a name=\"");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("\"></a>");
      } else if (command == "note" ||
          command == "abstract" ||
          command == "info" ||
          command == "tip" ||
          command == "success" ||
          command == "question" ||
          command == "warning" ||
          command == "failure" ||
          command == "danger" ||
          command == "bug" ||
          command == "example" ||
          command == "quote") {
        entity.docs.append("\n!!! ");
        entity.docs.append(command);
        entity.docs.append("\n");
        entity.indent += 4;
        entity.docs.append(entity.indent, ' ');
      } else if (command == "ingroup") {
        entity.ingroup = tokenizer.consume(WORD).str();

      /* legacy commands */
      } else if (command == "returns" ||
          command == "result") {
        entity.docs.append("\n:material-location-exit: **Return**\n:   ");
      } else if (command == "sa") {
        entity.docs.append("\n:material-eye-outline: **See**\n:   ");
      } else if (command == "file" ||
          command == "internal") {
        entity.hide = true;
      } else if (command == "brief" ||
          command == "short") {
        auto first = tokenizer.consume(~WHITESPACE);
        auto last = tokenizer.consume(PARA|CLOSE);
        entity.brief.append(first.first, last.first);
        entity.docs.append(first.first, last.first);
        entity.docs.append("\n\n");
      } else if (command == "e" ||
          command == "em" ||
          command == "a") {
        entity.docs.append("*");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("*");
      } else if (command == "b") {
        entity.docs.append("**");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("**");
      } else if (command == "c") {
        entity.docs.append("`");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`");
      } else if (command == "f$") {
        entity.docs.append("$");
      } else if (command == "f[" ||
          command == "f]") {
        entity.docs.append("$$");
      } else if (command == "li" ||
          command == "arg") {
        entity.docs.append("  - ");
      } else if (command == "ref") {
        auto href = tokenizer.consume(WORD);
        auto text = tokenizer.consume(WORD);
        entity.docs.append("[");
        entity.docs.append(text.str());
        entity.docs.append("](#");
        entity.docs.append(href.str());
        entity.docs.append(")");
      } else if (command == "code" ||
          command == "endcode" ||
          command == "verbatim" ||
          command == "endverbatim") {
        entity.docs.append("\n```");
      } else if (command == "attention") {
        entity.docs.append("\n!!! warning \"Attention\"\n");
        entity.indent = 4;
        entity.docs.append(entity.indent, ' ');
      } else if (command == "todo") {
        entity.docs.append("\n!!! example \"To-do\"\n");
        entity.indent = 4;
        entity.docs.append(entity.indent, ' ');
      } else if (command == "remark") {
        entity.docs.append("\n!!! quote \"Remark\"\n");
        entity.indent = 4;
        entity.docs.append(entity.indent, ' ');
      } else if (command == "def" ||
          command == "var" ||
          command == "fn" ||
          command == "class" ||
          command == "struct" ||
          command == "union" ||
          command == "enum" ||
          command == "typedef" ||
          command == "namespace" ||
          command == "interface" ||
          command == "protocol" ||
          command == "property") {
        /* ignore, including following name */
        tokenizer.consume(WORD);
      } else if (command == "@") {
        entity.docs.append("@");
      } else if (command == "/") {
        entity.docs.append("/");
      } else if (token.str().at(0) == '\\') {
        /* unrecognized command starting with legacy backslash, could just
         * be e.g. a LaTeX macro, output as is */
        entity.docs.append(token.str());
      } else {
        warn("unrecognized command: " << token.str());
        entity.docs.append(token.str());
      }
    } else if (token.type & PARA) {
      entity.docs.append("\n\n");
      entity.indent = std::max(entity.indent - 4, 0);
    } else if (token.type & LINE) {
      entity.docs.append("\n");
      entity.docs.append(entity.indent, ' ');
    } else if (token.type & CLOSE) {
      //
    } else {
      entity.docs.append(first*entity.indent, ' ');  // indent on first token
      entity.docs.append(token.str());
    }
    token = tokenizer.next();
    first = false;
  }

  /* always add a new line, as multiple doc comments may appear before an
   * entity (e.g. multiple end-of-line style comments) and these should be
   * separated with new lines */
  entity.docs.append("\n");
}
