#include "doxide/Parser.hpp"
#include "doxide/Tokenizer.hpp"

extern "C" const TSLanguage* tree_sitter_cpp();
void parseNode(const char* src, TSTreeCursor& cursor, Node& parent);
void interpret(const std::string_view& comment, Node& o);

const Node& Parser::root() const {
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
  parseNode(source.c_str(), cursor, global);
}

void parseNode(const char* src, TSTreeCursor& cursor, Node& parent) {
  TSNode node = ts_tree_cursor_current_node(&cursor);
  Node o;

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
    TSNode decl = ts_node_child(node, 2);
    if (!ts_node_is_null(decl)) {
      if (strcmp(ts_node_type(decl), "declaration") == 0) {
        decl = ts_node_child_by_field_name(decl, "declarator", 10);
        if (!ts_node_is_null(decl) &&
            strcmp(ts_node_type(decl), "function_declarator") == 0) {
          /* global or namespace template function declaration */
          TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

          uint32_t i = ts_node_start_byte(name);
          uint32_t j = ts_node_end_byte(name);
          uint32_t k = ts_node_start_byte(node);
          uint32_t l = ts_node_end_byte(node);

          if (strcmp(ts_node_type(name), "operator_name") == 0) {
            o.type = NodeType::OPERATOR;
          } else {
            o.type = NodeType::FUNCTION;
          }
          o.name = std::string_view{src + i, j - i};
          o.decl = std::string_view{src + k, l - k};

          parent.add(o);
        }
      } else if (strcmp(ts_node_type(decl), "function_definition") == 0) {
        decl = ts_node_child_by_field_name(decl, "declarator", 10);
        if (!ts_node_is_null(decl) &&
            strcmp(ts_node_type(decl), "declarator") == 0) {
          /* global, namespace or member function definition */
          TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);
          TSNode body = ts_node_child_by_field_name(decl, "body", 4);

          uint32_t i = ts_node_start_byte(name);
          uint32_t j = ts_node_end_byte(name);
          uint32_t k = ts_node_start_byte(node);
          uint32_t l = ts_node_start_byte(body);

          if (strcmp(ts_node_type(name), "operator_name") == 0) {
            o.type = NodeType::OPERATOR;
          } else {
            o.type = NodeType::FUNCTION;
          }
          o.name = std::string_view{src + i, j - i};
          o.decl = std::string_view{src + k, l - k};

          parent.add(o);
        }
      } else if (!ts_node_is_null(decl) &&
          (strcmp(ts_node_type(decl), "class_specifier") == 0 ||
          strcmp(ts_node_type(decl), "struct_specifier") == 0)) {
        /* class */
        TSNode name = ts_node_child_by_field_name(decl, "name", 4);
        TSNode body = ts_node_child_by_field_name(decl, "body", 4);

        uint32_t i = ts_node_start_byte(name);
        uint32_t j = ts_node_end_byte(name);
        uint32_t k = ts_node_start_byte(node);
        uint32_t l = ts_node_start_byte(body);

        o.type = NodeType::TYPE;
        o.name = std::string_view{src + i, j - i};
        o.decl = std::string_view{src + k, l - k};

        if (ts_tree_cursor_goto_first_child(&cursor)) {
          parseNode(src, cursor, o);
          ts_tree_cursor_goto_parent(&cursor);
        }
        parent.add(o);
      }
    }
  } else if (strcmp(ts_node_type(node), "namespace_definition") == 0) {
    /* namespace */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode body = ts_node_child_by_field_name(node, "body", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_start_byte(body);

    o.type = NodeType::NAMESPACE;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};

    if (ts_tree_cursor_goto_first_child(&cursor)) {
      parseNode(src, cursor, o);
      ts_tree_cursor_goto_parent(&cursor);
    }
    parent.add(o);
  } else if (strcmp(ts_node_type(node), "class_specifier") == 0 ||
      strcmp(ts_node_type(node), "struct_specifier") == 0) {
    /* class */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode body = ts_node_child_by_field_name(node, "body", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_start_byte(body);

    o.type = NodeType::TYPE;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};

    if (ts_tree_cursor_goto_first_child(&cursor)) {
      parseNode(src, cursor, o);
      ts_tree_cursor_goto_parent(&cursor);
    }
    parent.add(o);
  } else if (strcmp(ts_node_type(node), "declaration") == 0) {
    TSNode decl = ts_node_child_by_field_name(node, "declarator", 10);
    if (strcmp(ts_node_type(decl), "identifier") == 0) {
      /* global or namespace variable */
      TSNode name = decl;

      uint32_t i = ts_node_start_byte(name);
      uint32_t j = ts_node_end_byte(name);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);

      o.type = NodeType::VARIABLE;
      o.name = std::string_view{src + i, j - i};
      o.decl = std::string_view{src + k, l - k};

      parent.add(o);
    } else if (strcmp(ts_node_type(decl), "init_declarator") == 0) {
      /* global or namespace variable */
      TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

      uint32_t i = ts_node_start_byte(name);
      uint32_t j = ts_node_end_byte(name);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);

      o.type = NodeType::VARIABLE;
      o.name = std::string_view{src + i, j - i};
      o.decl = std::string_view{src + k, l - k};

      parent.add(o);
    } else if (strcmp(ts_node_type(decl), "function_declarator") == 0) {
      /* global or namespace function */
      TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

      uint32_t i = ts_node_start_byte(name);
      uint32_t j = ts_node_end_byte(name);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);

      if (strcmp(ts_node_type(name), "operator_name") == 0) {
        o.type = NodeType::OPERATOR;
      } else {
        o.type = NodeType::FUNCTION;
      }
      o.name = std::string_view{src + i, j - i};
      o.decl = std::string_view{src + k, l - k};

      parent.add(o);
    }
  } else if (strcmp(ts_node_type(node), "field_declaration") == 0) {
    /* member variable */
    TSNode name = ts_node_child_by_field_name(node, "declarator", 10);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);

    o.type = NodeType::VARIABLE;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};

    parent.add(o);
  } else if (strcmp(ts_node_type(node), "function_definition") == 0) {
    /* global, namespace or member function */
    TSNode name = ts_node_child_by_field_name(ts_node_child_by_field_name(
        node, "declarator", 10), "declarator", 10);
    TSNode body = ts_node_child_by_field_name(node, "body", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_start_byte(body);

    if (strcmp(ts_node_type(name), "operator_name") == 0) {
      o.type = NodeType::OPERATOR;
    } else {
      o.type = NodeType::FUNCTION;
    }
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};

    parent.add(o);
  } else if (strcmp(ts_node_type(node), "preproc_def") == 0) {
    /* macro definition */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = j;

    o.type = NodeType::MACRO;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};

    parent.add(o);
  } else if (strcmp(ts_node_type(node), "preproc_function_def") == 0) {
    /* macro definition with arguments */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode params = ts_node_child_by_field_name(node, "parameters", 10);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(params);

    o.type = NodeType::MACRO;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};

    parent.add(o);
  } else {
    /* continue depth-first search */
    if (ts_tree_cursor_goto_first_child(&cursor)) {
      parseNode(src, cursor, parent);
      ts_tree_cursor_goto_parent(&cursor);
    }
  }

  /* next sibling */
  if (ts_tree_cursor_goto_next_sibling(&cursor)) {
    parseNode(src, cursor, parent);
  }
}

void interpret(const std::string_view& comment, Node& o) {
  int indent = 0;
  Tokenizer tokenizer(comment);
  Token token = tokenizer.next();
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
          Node group;
          group.type = NodeType::GROUP;
          group.name = tokenizer.consume(WORD).str();
          o.add(group);
          o.docs.append(":material-view-module-outline: **Group** [");
          o.docs.append(group.name);
          o.docs.append("](");
          o.docs.append(group.name);
          o.docs.append("/)\n:   ");
        } else if (token.substr(1) == "ingroup") {
          o.ingroup = tokenizer.consume(WORD).str();

        /* legacy commands */
        } else if (token.substr(1) == "returns" ||
            token.substr(1) == "result") {
          o.docs.append(":material-location-exit: **Return**\n:   ");
        } else if (token.substr(1) == "sa") {
          o.docs.append(":material-eye-outline: **See**\n:   ");
        } else if (token.substr(1) == "file") {
          o.type = NodeType::FILE;
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
