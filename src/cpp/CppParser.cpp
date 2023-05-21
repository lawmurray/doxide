#include "cpp/CppParser.hpp"
#include "doc/DocParser.hpp"
#include "doc/DocTokenizer.hpp"

extern "C" const TSLanguage* tree_sitter_cpp();

const Entity& Parser::root() const {
  return global;
}

void Parser::parse(const std::string& file) {
  /* read file */
  std::string source;
  std::ifstream in(file);
  char buffer[16384];
  while (in.read(buffer, sizeof(buffer))) {
    source.append(buffer, sizeof(buffer));
  }
  source.append(buffer, in.gcount());

  /* set up parser */
  TSParser* parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_cpp());
  TSTree *tree = ts_parser_parse_string(parser, NULL, source.c_str(),
      source.size());

  /* traverse syntax tree */
  TSTreeCursor cursor = ts_tree_cursor_new(ts_tree_root_node(tree));
  parseEntity(source.c_str(), cursor, global);
}

void Parser::parseEntity(const char* src, TSTreeCursor& cursor, Entity& parent) {
  TSNode node = ts_tree_cursor_current_node(&cursor);
  Entity o;

  /* handle comments */
  while (strcmp(ts_node_type(node), "comment") == 0) {
    /* possibly a documentation comment */
    uint32_t i = ts_node_start_byte(node);
    uint32_t j = ts_node_end_byte(node);
    interpret(std::string_view(src + i, j - i), o);

    /* advance to next sibling to obtain documented entity */
    ts_tree_cursor_goto_next_sibling(&cursor);
    node = ts_tree_cursor_current_node(&cursor);
  }

  if (strcmp(ts_node_type(node), "template_declaration") == 0) {
    /* template */
    TSNode decl = ts_node_named_child(node, 1);
    if (!ts_node_is_null(decl)) {
      if (strcmp(ts_node_type(decl), "declaration") == 0 ||
          strcmp(ts_node_type(decl), "function_definition") == 0) {
        decl = ts_node_child_by_field_name(decl, "declarator", 10);
        if (!ts_node_is_null(decl) &&
            strcmp(ts_node_type(decl), "function_declarator") == 0) {
          /* function template declaration */
          TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

          uint32_t i = ts_node_start_byte(name);
          uint32_t j = ts_node_end_byte(name);
          uint32_t k = ts_node_start_byte(node);
          uint32_t l = ts_node_end_byte(decl);

          if (strcmp(ts_node_type(name), "operator_name") == 0) {
            o.type = EntityType::OPERATOR;
          } else {
            o.type = EntityType::FUNCTION;
          }
          o.name = std::string_view{src + i, j - i};
          o.decl = std::string_view{src + k, l - k};
        }
      } else if (!ts_node_is_null(decl) &&
          (strcmp(ts_node_type(decl), "class_specifier") == 0 ||
          strcmp(ts_node_type(decl), "struct_specifier") == 0 ||
          strcmp(ts_node_type(decl), "union_specifier") == 0 ||
          strcmp(ts_node_type(decl), "enum_specifier") == 0 ||
          strcmp(ts_node_type(decl), "alias_declaration") == 0)) {
        /* class template */
        TSNode name = ts_node_child_by_field_name(decl, "name", 4);
        TSNode body = ts_node_child_by_field_name(decl, "body", 4);

        uint32_t i = ts_node_start_byte(name);
        uint32_t j = ts_node_end_byte(name);
        uint32_t k = ts_node_start_byte(node);
        uint32_t l = ts_node_is_null(body) ?
            ts_node_end_byte(node) : ts_node_start_byte(body);

        o.type = EntityType::TYPE;
        o.name = std::string_view{src + i, j - i};
        o.decl = std::string_view{src + k, l - k};

        if (ts_tree_cursor_goto_first_child(&cursor)) {
          parseEntity(src, cursor, o);
          ts_tree_cursor_goto_parent(&cursor);
        }
      } else if (!ts_node_is_null(decl) &&
          strcmp(ts_node_type(node), "type_definition") == 0) {
        /* class template */
        TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

        uint32_t i = ts_node_start_byte(name);
        uint32_t j = ts_node_end_byte(name);
        uint32_t k = ts_node_start_byte(node);
        uint32_t l = ts_node_end_byte(node);

        o.type = EntityType::TYPE;
        o.name = std::string_view{src + i, j - i};
        o.decl = std::string_view{src + k, l - k};
      }
    }
  } else if (strcmp(ts_node_type(node), "namespace_definition") == 0) {
    /* namespace */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    if (!ts_node_is_null(name)) {
      Entity* ns = &parent;
      if (strcmp(ts_node_type(name), "nested_namespace_specifier") == 0) {
        /* nested namespaces */
        uint32_t n = ts_node_named_child_count(name);
        for (uint32_t i = 0; i < n; ++i) {
          TSNode child = ts_node_named_child(name, i);
          uint32_t k = ts_node_start_byte(child);
          uint32_t l = ts_node_end_byte(child);
          ns = &ns->ns(std::string{src + k, l - k});
        }
      } else {
        /* namespace */
        uint32_t i = ts_node_start_byte(name);
        uint32_t j = ts_node_end_byte(name);
        ns = &ns->ns(std::string{src + i, j - i});
      }
      if (ns->docs.empty()) {
        ns->docs = o.docs;
      }
      if (ns->brief.empty()) {
        ns->brief = o.docs;
      }
      if (ns->ingroup.empty()) {
        ns->ingroup = o.ingroup;
      }
      ns->hide = ns->hide || o.hide;
      if (ts_tree_cursor_goto_first_child(&cursor)) {
        parseEntity(src, cursor, *ns);
        ts_tree_cursor_goto_parent(&cursor);
      }
    }
  } else if (strcmp(ts_node_type(node), "class_specifier") == 0 ||
      strcmp(ts_node_type(node), "struct_specifier") == 0 ||
      strcmp(ts_node_type(node), "union_specifier") == 0 ||
      strcmp(ts_node_type(node), "enum_specifier") == 0 ||
      strcmp(ts_node_type(node), "alias_declaration") == 0) {
    /* class */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode body = ts_node_child_by_field_name(node, "body", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_is_null(body) ?
        ts_node_end_byte(node) : ts_node_start_byte(body);

    o.type = EntityType::TYPE;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};

    if (ts_tree_cursor_goto_first_child(&cursor)) {
      parseEntity(src, cursor, o);
      ts_tree_cursor_goto_parent(&cursor);
    }
  } else if (strcmp(ts_node_type(node), "declaration") == 0 ||
      strcmp(ts_node_type(node), "function_definition") == 0) {
    TSNode decl = ts_node_child_by_field_name(node, "declarator", 10);
    if (strcmp(ts_node_type(decl), "identifier") == 0) {
      /* global or namespace variable */
      TSNode name = decl;

      uint32_t i = ts_node_start_byte(name);
      uint32_t j = ts_node_end_byte(name);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);

      o.type = EntityType::VARIABLE;
      o.name = std::string_view{src + i, j - i};
      o.decl = std::string_view{src + k, l - k};
    } else if (strcmp(ts_node_type(decl), "init_declarator") == 0) {
      /* global or namespace variable */
      TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

      uint32_t i = ts_node_start_byte(name);
      uint32_t j = ts_node_end_byte(name);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);

      o.type = EntityType::VARIABLE;
      o.name = std::string_view{src + i, j - i};
      o.decl = std::string_view{src + k, l - k};
    } else if (strcmp(ts_node_type(decl), "function_declarator") == 0) {
      /* global or namespace function */
      TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

      uint32_t i = ts_node_start_byte(name);
      uint32_t j = ts_node_end_byte(name);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(decl);

      if (strcmp(ts_node_type(name), "operator_name") == 0) {
        o.type = EntityType::OPERATOR;
      } else {
        o.type = EntityType::FUNCTION;
      }
      o.name = std::string_view{src + i, j - i};
      o.decl = std::string_view{src + k, l - k};
    }
  } else if (strcmp(ts_node_type(node), "field_declaration") == 0) {
    /* member variable */
    TSNode name = ts_node_child_by_field_name(node, "declarator", 10);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);

    o.type = EntityType::VARIABLE;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
  } else if (strcmp(ts_node_type(node), "enumerator") == 0) {
    /* enumerator */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);

    o.type = EntityType::ENUMERATOR;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
  } else if (strcmp(ts_node_type(node), "preproc_def") == 0) {
    /* macro */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = j;

    o.type = EntityType::MACRO;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
  } else if (strcmp(ts_node_type(node), "preproc_function_def") == 0) {
    /* macro definition with arguments */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode params = ts_node_child_by_field_name(node, "parameters", 10);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(params);

    o.type = EntityType::MACRO;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
  } else {
    /* continue depth-first search */
    if (ts_tree_cursor_goto_first_child(&cursor)) {
      parseEntity(src, cursor, parent);
      ts_tree_cursor_goto_parent(&cursor);
    }
  }

  /* register node */
  if (o.type != EntityType::NONE) {
    if (o.ingroup.empty()) {
      parent.add(o);
    } else {
      global.add(o);
    }
  }

  /* next sibling */
  if (ts_tree_cursor_goto_next_sibling(&cursor)) {
    parseEntity(src, cursor, parent);
  }
}

void Parser::interpret(const std::string_view& comment, Entity& o) {
  int indent = 0;
  DocTokenizer tokenizer(comment);
  DocToken token = tokenizer.next();
  o.docs.clear();
  if (token.type & DOC) {  // otherwise not a documentation comment
    token = tokenizer.next();
    while (token.type) {
      if (token.type & DOC_COMMAND) {
        /* non-legacy commands */
        if (token.substr(1) == "param" ||
            token.substr(1) == "param[in]") {
          o.docs.append(":material-location-enter: **Parameter** `");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("`\n:   ");
        } else if (token.substr(1) == "param[out]") {
          o.docs.append(":material-location-exit: **Parameter** `");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("`\n:   ");
        } else if (token.substr(1) == "param[in,out]") {
          o.docs.append(":material-location-enter::material-location-exit: **Parameter** `");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("`\n:   ");
        } else if (token.substr(1) == "tparam") {
          o.docs.append(":material-code-tags: **Template parameter** `");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("`\n:   ");
        } else if (token.substr(1) == "p") {
          o.docs.append("`");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("`");
        } else if (token.substr(1) == "return") {
          o.docs.append(":material-location-exit: **Return**\n:   ");
        } else if (token.substr(1) == "pre") {
          o.docs.append(":material-check-circle-outline: **Pre-condition**\n:   ");
        } else if (token.substr(1) == "post") {
          o.docs.append(":material-check-circle-outline: **Post-condition**\n:   ");
        } else if (token.substr(1) == "throw") {
          o.docs.append(":material-alert-circle-outline: **Throw**\n:   ");
        } else if (token.substr(1) == "see") {
          o.docs.append(":material-eye-outline: **See**\n:   ");
        } else if (token.substr(1) == "anchor") {
          o.docs.append("<a name=\"");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("\"></a>");
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
          o.docs.append("!!! ");
          o.docs.append(token.substr(1));
          indent += 4;
          o.docs.append(indent, ' ');
        } else if (token.substr(1) == "group") {
          Entity group;
          group.type = EntityType::GROUP;
          group.name = tokenizer.consume(WORD).str();
          auto first = tokenizer.consume(~WHITESPACE);
          auto last = tokenizer.consume(DOC_PARA|DOC_CLOSE);
          if (first.type && last.type) {
            group.docs.append(first.first, last.first);
          }
          global.add(group);
        } else if (token.substr(1) == "ingroup") {
          o.ingroup = tokenizer.consume(WORD).str();

        /* legacy commands */
        } else if (token.substr(1) == "returns" ||
            token.substr(1) == "result") {
          o.docs.append(":material-location-exit: **Return**\n:   ");
        } else if (token.substr(1) == "sa") {
          o.docs.append(":material-eye-outline: **See**\n:   ");
        } else if (token.substr(1) == "file") {
          o.type = EntityType::FILE;
        } else if (token.substr(1) == "internal") {
          o.hide = true;
        } else if (token.substr(1) == "brief" ||
            token.substr(1) == "short") {
          auto first = tokenizer.consume(~WHITESPACE);
          auto last = tokenizer.consume(SENTENCE);
          o.brief.append(first.first, last.last);
        } else if (token.substr(1) == "e" ||
            token.substr(1) == "em" ||
            token.substr(1) == "a") {
          o.docs.append("*");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("*");
        } else if (token.substr(1) == "b") {
          o.docs.append("**");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("**");
        } else if (token.substr(1) == "c") {
          o.docs.append("`");
          o.docs.append(tokenizer.consume(WORD).str());
          o.docs.append("`");
        } else if (token.substr(1) == "f$") {
          o.docs.append("$");
        } else if (token.substr(1) == "f[" ||
            token.substr(1) == "f]") {
          o.docs.append("$$");
        } else if (token.substr(1) == "li" ||
            token.substr(1) == "arg") {
          o.docs.append("  - ");
        } else if (token.substr(1) == "ref") {
          auto href = tokenizer.consume(WORD);
          auto text = tokenizer.consume(WORD);
          o.docs.append("[");
          o.docs.append(text.str());
          o.docs.append("](#");
          o.docs.append(href.str());
          o.docs.append(")");
        } else if (token.substr(1) == "code" ||
            token.substr(1) == "endcode" ||
            token.substr(1) == "verbatim" ||
            token.substr(1) == "endverbatim") {
          o.docs.append("```");
        } else if (token.substr(1) == "attention") {
          o.docs.append("!!! warning \"Attention\"\n");
          indent += 4;
          o.docs.append(indent, ' ');
        } else if (token.substr(1) == "todo") {
          o.docs.append("!!! example \"To-do\"\n");
          indent += 4;
          o.docs.append(indent, ' ');
        } else if (token.substr(1) == "remark") {
          o.docs.append("!!! quote \"Remark\"\n");
          indent += 4;
          o.docs.append(indent, ' ');

        /* unrecognized commands */
        } else {
          warn("unrecognized command: " << token.str());
          o.docs.append(token.str());
        }
      } else if (token.type & DOC_ESCAPE) {
        o.docs.append(token.substr(1));
      } else if (token.type & DOC_PARA) {
        o.docs.append("\n\n");
        indent = std::max(indent - 4, 0);
      } else if (token.type & DOC_LINE) {
        o.docs.append("\n");
        o.docs.append(indent, ' ');
      } else if (token.type & DOC_CLOSE) {
        //
      } else {
        o.docs.append(token.str());
      }
      token = tokenizer.next();
    }
  }
}
