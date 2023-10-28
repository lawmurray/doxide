#include "Parser.hpp"
#include "Tokenizer.hpp"
#include "Language.hpp"

Parser::Parser() :
    parser(nullptr),
    query(nullptr),
    parserPreprocess(nullptr),
    queryPreprocess(nullptr) {
  uint32_t error_offset;
  TSQueryError error_type;

  /* parser */
  parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_cpp());

  /* query */
  query = ts_query_new(tree_sitter_cpp(), query_cpp, strlen(query_cpp),
      &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from(query_cpp + error_offset, 40);
    error("invalid query starting " << from << "...");
  }

  /* preprocessor parser */
  parserPreprocess = ts_parser_new();
  ts_parser_set_language(parserPreprocess, tree_sitter_cpp());

  /* preprocessor query */
  queryPreprocess = ts_query_new(tree_sitter_cpp(), query_preprocess,
      strlen(query_preprocess), &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from(query_preprocess + error_offset, 40);
    error("invalid query starting " << from << "...");
  }
}

Parser::~Parser() {
  ts_query_delete(query);
  ts_parser_delete(parser);
  ts_query_delete(queryPreprocess);
  ts_parser_delete(parserPreprocess);
}

void Parser::parse(const std::string& file, Entity& global) {
  std::string in = preprocess(file);
  ts_parser_reset(parser);
  TSTree* tree = ts_parser_parse_string(parser, NULL, in.data(), in.size());
  if (!tree) {
    warn("cannot parse " << file << ", skipping");
  }
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
        if (token.type == AFTER_OPEN) {
          translate(docs, entities.back());
        } else if (token.type == BEFORE_OPEN) {
          translate(docs, entity);
        }
      } else if (strncmp(name, "name", length) == 0) {
        entity.name = in.substr(k, l - k);
      } else if (strncmp(name, "body", length) == 0) {
        middle = ts_node_start_byte(node);
      } else if (strncmp(name, "value", length) == 0) {
        middle = ts_node_start_byte(node);
      } else {
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
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
}

std::string Parser::preprocess(const std::string& file) {
  static std::regex macro(R"([A-Z_][A-Z0-9_]{2,})");

  std::string in = gulp(file);
  ts_parser_reset(parserPreprocess);
  TSTree* tree = ts_parser_parse_string(parserPreprocess, NULL, in.data(),
      in.size());
  TSNode root = ts_tree_root_node(tree);
  TSTreeCursor cursor = ts_tree_cursor_new(root);
  TSNode node = ts_tree_cursor_current_node(&cursor);
  do {
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);
    if (ts_node_is_error(node)) {
      /* parse error: assuming that the syntax is actually valid, this is
       * usually caused by use of preprocessor macros, as the preprocessor is
       * not run */
      TSPoint from = ts_node_start_point(node);
      TSPoint to = ts_node_end_point(node);

      std::cerr << file << ':' << (from.row + 1) << ':' << from.column <<
          ": warning: parse failed at '" << in.substr(k, l - k) << "'" <<
          std::endl;

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
        std::cerr << file << ':' << (from.row + 1) << ':' << from.column <<
            ": note: attempting recovery by erasing '" <<
            in.substr(k, l - k) << "'" << std::endl;

        /* overwrite with whitespace, rather than erasing entirely, to
         * preserve line and column numbers from user's perspective */
        in.replace(k, l - k, l - k, ' ');

        /* update cursor state */
        ts_tree_cursor_delete(&cursor);
        TSInputEdit edit{k, l, l, from, to, to};
        ts_tree_edit(tree, &edit);
        ts_node_edit(&root, &edit);
        ts_node_edit(&node, &edit);
        cursor = ts_tree_cursor_new(node);
        break;  // try again
      } else {
        /* go back to where we were */
        warn("recovery failed, proceeding anyway");
        while (back--) {
          ts_tree_cursor_goto_next_sibling(&cursor);
        }
      }
    }

    /* next node */
    if (ts_tree_cursor_goto_first_child(&cursor)) {
      //
    } else if (ts_tree_cursor_goto_next_sibling(&cursor)) {
      //
    } else while (ts_tree_cursor_goto_parent(&cursor) &&
        !ts_tree_cursor_goto_next_sibling(&cursor)) {
      //
    }
    node = ts_tree_cursor_current_node(&cursor);
  } while (!ts_node_eq(node, root));

  ts_tree_cursor_delete(&cursor);
  ts_tree_delete(tree);
  return in;
}

void Parser::translate(const std::string_view& comment, Entity& entity) {
  int indent = 0;
  bool file = false;  // does this contain @file?
  Tokenizer tokenizer(comment);
  Token token = tokenizer.next();
  token = tokenizer.next();
  while (token.type) {
    if (token.type & COMMAND) {
      /* non-legacy commands */
      if (token.substr(1) == "param" ||
          token.substr(1) == "param[in]") {
        entity.docs.append(":material-location-enter: **Parameter** `");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`\n:   ");
        indent = 4;
      } else if (token.substr(1) == "param[out]") {
        entity.docs.append(":material-location-exit: **Parameter** `");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`\n:   ");
        indent = 4;
      } else if (token.substr(1) == "param[in,out]") {
        entity.docs.append(":material-location-enter::material-location-exit: **Parameter** `");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`\n:   ");
        indent = 4;
      } else if (token.substr(1) == "tparam") {
        entity.docs.append(":material-code-tags: **Template parameter** `");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`\n:   ");
        indent = 4;
      } else if (token.substr(1) == "p") {
        entity.docs.append("`");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`");
      } else if (token.substr(1) == "return") {
        entity.docs.append(":material-keyboard-return: **Return**\n:   ");
      } else if (token.substr(1) == "pre") {
        entity.docs.append(":material-check-circle-outline: **Pre-condition**\n:   ");
      } else if (token.substr(1) == "post") {
        entity.docs.append(":material-check-circle-outline: **Post-condition**\n:   ");
      } else if (token.substr(1) == "throw") {
        entity.docs.append(":material-alert-circle-outline: **Throw**\n:   ");
      } else if (token.substr(1) == "see") {
        entity.docs.append(":material-eye-outline: **See**\n:   ");
      } else if (token.substr(1) == "anchor") {
        entity.docs.append("<a name=\"");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("\"></a>");
      } else if (token.substr(1) == "note" ||
          token.substr(1) == "abstract" ||
          token.substr(1) == "info" ||
          token.substr(1) == "tip" ||
          token.substr(1) == "success" ||
          token.substr(1) == "question" ||
          token.substr(1) == "warning" ||
          token.substr(1) == "failure" ||
          token.substr(1) == "danger" ||
          token.substr(1) == "bug" ||
          token.substr(1) == "example" ||
          token.substr(1) == "quote") {
        entity.docs.append("!!! ");
        entity.docs.append(token.substr(1));
        entity.docs.append("\n");
        indent += 4;
        entity.docs.append(indent, ' ');
      } else if (token.substr(1) == "ingroup") {
        entity.ingroup = tokenizer.consume(WORD).str();

      /* legacy commands */
      } else if (token.substr(1) == "returns" ||
          token.substr(1) == "result") {
        entity.docs.append(":material-location-exit: **Return**\n:   ");
      } else if (token.substr(1) == "sa") {
        entity.docs.append(":material-eye-outline: **See**\n:   ");
      } else if (token.substr(1) == "file") {
        file = true;
      } else if (token.substr(1) == "internal") {
        entity.hide = true;
      } else if (token.substr(1) == "brief" ||
          token.substr(1) == "short") {
        auto first = tokenizer.consume(~WHITESPACE);
        auto last = tokenizer.consume(PARA|CLOSE);
        entity.brief.append(first.first, last.first);
        entity.docs.append(first.first, last.first);
        entity.docs.append("\n\n");
      } else if (token.substr(1) == "e" ||
          token.substr(1) == "em" ||
          token.substr(1) == "a") {
        entity.docs.append("*");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("*");
      } else if (token.substr(1) == "b") {
        entity.docs.append("**");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("**");
      } else if (token.substr(1) == "c") {
        entity.docs.append("`");
        entity.docs.append(tokenizer.consume(WORD).str());
        entity.docs.append("`");
      } else if (token.substr(1) == "f$") {
        entity.docs.append("$");
      } else if (token.substr(1) == "f[" ||
          token.substr(1) == "f]") {
        entity.docs.append("$$");
      } else if (token.substr(1) == "li" ||
          token.substr(1) == "arg") {
        entity.docs.append("  - ");
      } else if (token.substr(1) == "ref") {
        auto href = tokenizer.consume(WORD);
        auto text = tokenizer.consume(WORD);
        entity.docs.append("[");
        entity.docs.append(text.str());
        entity.docs.append("](#");
        entity.docs.append(href.str());
        entity.docs.append(")");
      } else if (token.substr(1) == "code" ||
          token.substr(1) == "endcode" ||
          token.substr(1) == "verbatim" ||
          token.substr(1) == "endverbatim") {
        entity.docs.append("```");
      } else if (token.substr(1) == "attention") {
        entity.docs.append("!!! warning \"Attention\"\n");
        indent += 4;
        entity.docs.append(indent, ' ');
      } else if (token.substr(1) == "todo") {
        entity.docs.append("!!! example \"To-do\"\n");
        indent += 4;
        entity.docs.append(indent, ' ');
      } else if (token.substr(1) == "remark") {
        entity.docs.append("!!! quote \"Remark\"\n");
        indent += 4;
        entity.docs.append(indent, ' ');
      } else if (token.substr(1) == "def" ||
          token.substr(1) == "var" ||
          token.substr(1) == "fn" ||
          token.substr(1) == "class" ||
          token.substr(1) == "struct" ||
          token.substr(1) == "union" ||
          token.substr(1) == "enum" ||
          token.substr(1) == "typedef" ||
          token.substr(1) == "namespace" ||
          token.substr(1) == "interface" ||
          token.substr(1) == "protocol" ||
          token.substr(1) == "property") {
        /* ignore, including following name */
        tokenizer.consume(WORD);
      } else if (token.substr(1) == "@") {
        entity.docs.append("@");
      } else if (token.substr(1) == "/") {
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
      indent = std::max(indent - 4, 0);
    } else if (token.type & LINE) {
      entity.docs.append("\n");
      entity.docs.append(indent, ' ');
    } else if (token.type & CLOSE) {
      //
    } else {
      entity.docs.append(token.str());
    }
    token = tokenizer.next();
  }
  if (file) {
    entity.docs.clear();
  }
}
