#include "doxide/Parser.hpp"

extern "C" const TSLanguage* tree_sitter_cpp();
void parseNode(const char* src, TSTreeCursor* cursor, Node* parent);

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
  parseNode(source.c_str(), &cursor, &global);
}

void parseNode(const char* src, TSTreeCursor* cursor, Node* parent) {
  TSNode node = ts_tree_cursor_current_node(cursor);
  Node o;
  
  if (strcmp(ts_node_type(node), "comment") == 0) {
    uint32_t i = ts_node_start_byte(node);
    uint32_t j = ts_node_end_byte(node);
    o.docs = std::string_view(src + i, j - i);

    /* advance to next sibling to obtain documented entity */
    ts_tree_cursor_goto_next_sibling(cursor);
    node = ts_tree_cursor_current_node(cursor);
  }
  if (strcmp(ts_node_type(node), "namespace_definition") == 0) {
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode body = ts_node_child_by_field_name(node, "body", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_start_byte(body);

    o.type = NodeType::NAMESPACE;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
    parent->add(o);

    std::cerr << o.docs << std::endl;
    std::cerr << "namespace " << o.name << ": " << o.decl << std::endl;

    if (ts_tree_cursor_goto_first_child(cursor)) {
      parseNode(src, cursor, &o);
      ts_tree_cursor_goto_parent(cursor);
    }
  } else if (strcmp(ts_node_type(node), "class_specifier") == 0) {
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode body = ts_node_child_by_field_name(node, "body", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_start_byte(body);

    o.type = NodeType::TYPE;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
    parent->add(o);

    std::cerr << o.docs << std::endl;
    std::cerr << "class " << o.name << ": " << o.decl << std::endl;

    if (ts_tree_cursor_goto_first_child(cursor)) {
      parseNode(src, cursor, &o);
      ts_tree_cursor_goto_parent(cursor);
    }
  } else if (strcmp(ts_node_type(node), "field_declaration") == 0 ||
      strcmp(ts_node_type(node), "declaration") == 0) {
    TSNode name = ts_node_child_by_field_name(node, "declarator", 10);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);

    o.type = NodeType::VARIABLE;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
    parent->add(o);

    std::cerr << o.docs << std::endl;
    std::cerr << "variable " << o.name << ": " << o.decl << std::endl;
  } else if (strcmp(ts_node_type(node), "function_definition") == 0) {
    TSNode name = ts_node_child_by_field_name(ts_node_child_by_field_name(node, "declarator", 10), "declarator", 10);
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
    parent->add(o);

    std::cerr << o.docs << std::endl;
    if (strcmp(ts_node_type(name), "operator_name") == 0) {
      std::cerr << "operator " << o.name << ": " << o.decl << std::endl;
    } else {
      std::cerr << "function " << o.name << ": " << o.decl << std::endl;
    }
  } else if (strcmp(ts_node_type(node), "preproc_def") == 0) {
    TSNode name = ts_node_child_by_field_name(node, "name", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = j;

    o.type = NodeType::MACRO;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
    parent->add(o);

    std::cerr << o.docs << std::endl;
    std::cerr << "macro " << o.name << ": " << o.decl << std::endl;
  } else if (strcmp(ts_node_type(node), "preproc_function_def") == 0) {
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode params = ts_node_child_by_field_name(node, "parameters", 10);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(params);

    o.type = NodeType::MACRO;
    o.name = std::string_view{src + i, j - i};
    o.decl = std::string_view{src + k, l - k};
    parent->add(o);

    std::cerr << o.docs << std::endl;
    std::cerr << "macro " << o.name << ": " << o.decl << std::endl;
  } else {
    /* continue depth-first search */
    if (ts_tree_cursor_goto_first_child(cursor)) {
      parseNode(src, cursor, parent);
      ts_tree_cursor_goto_parent(cursor);
    }
  }

  /* next sibling */
  if (ts_tree_cursor_goto_next_sibling(cursor)) {
    parseNode(src, cursor, parent);
  }
}

Token Parser::consume(const uint64_t stop) {
  Token token = tokenizer.next();
  while (token.type && !(token.type & stop)) {
    if (token.type & (BRACE|BRACKET|PAREN)) {
      /* consume to a matching close, which is one left shift away */
      token = consume(token.type << 1);
    } else if (token.type & ANGLE) {
      /* ambiguity with operators < > <= >=, consume to any close */
      token = consume(BRACE_CLOSE|BRACKET_CLOSE|PAREN_CLOSE|ANGLE_CLOSE);
      if (!(token.type & ANGLE_CLOSE)) {
        /* unmatched close, treat as an operator not a delimiter */
        return token;
      }
    }
    token = tokenizer.next();
  }
  return token;
}

std::string_view Parser::consumeWord() {
  Token token;
  do {
    token = tokenizer.next();
  } while (token.type && (token.type & WHITESPACE));
  return token.str();
}

std::string_view Parser::consumeSentence() {
  Token first = tokenizer.next();
  while (first.type && (first.type & WHITESPACE)) {
    first = tokenizer.next();
  }
  Token last = first;
  while (last.type && !(last.type & (SENTENCE|DOC_CLOSE))) {
    last = tokenizer.next();
  } 
  return std::string_view(first.first, last.last);
}

std::string_view Parser::consumeParagraph() {
  Token first = tokenizer.next();
  while (first.type && (first.type & WHITESPACE)) {
    first = tokenizer.next();
  }
  Token last = first;
  while (last.type && !(last.type & (DOC_PARA|DOC_CLOSE))) {
    last = tokenizer.next();
  }
  return std::string_view(first.first, last.last);
}

Node Parser::interpret() {
  Node node;
  Token token = tokenizer.next();
  int indent = 0;
  while (token.type && !(token.type & DOC_CLOSE)) {
    if (token.type & DOC_COMMAND) {
      /* non-legacy commands */
      if (token.substr(1) == "param" ||
          token.substr(1) == "param[in]") {
        node.docs.append(":material-location-enter: **Parameter** `");
        node.docs.append(consumeWord());
        node.docs.append("`\n:   ");
      } else if (token.substr(1) == "param[out]") {
        node.docs.append(":material-location-exit: **Parameter** `");
        node.docs.append(consumeWord());
        node.docs.append("`\n:   ");
      } else if (token.substr(1) == "param[in,out]") {
        node.docs.append(":material-location-enter::material-location-exit: **Parameter** `");
        node.docs.append(consumeWord());
        node.docs.append("`\n:   ");
      } else if (token.substr(1) == "tparam") {
        node.docs.append(":material-code-tags: **Template parameter** `");
        node.docs.append(consumeWord());
        node.docs.append("`\n:   ");
      } else if (token.substr(1) == "p") {
        node.docs.append("`");
        node.docs.append(consumeWord());
        node.docs.append("`");
      } else if (token.substr(1) == "return") {
        node.docs.append(":material-location-exit: **Return**\n:   ");
      } else if (token.substr(1) == "pre") {
        node.docs.append(":material-check-circle-outline: **Pre-condition**\n:   ");
      } else if (token.substr(1) == "post") {
        node.docs.append(":material-check-circle-outline: **Post-condition**\n:   ");
      } else if (token.substr(1) == "throw") {
        node.docs.append(":material-alert-circle-outline: **Throw**\n:   ");
      } else if (token.substr(1) == "see") {
        node.docs.append(":material-eye-outline: **See**\n:   ");
      } else if (token.substr(1) == "anchor") {
        node.docs.append("<a name=\"");
        node.docs.append(consumeWord());
        node.docs.append("\"></a>");
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
        node.docs.append("!!! ");
        node.docs.append(token.substr(1));
        indent += 4;
        node.docs.append(indent, ' ');
      } else if (token.substr(1) == "group") {
        Node group;
        group.type = NodeType::GROUP;
        group.name = consumeWord();
        node.add(group);
        node.docs.append(":material-view-module-outline: **Group** [");
        node.docs.append(group.name);
        node.docs.append("](");
        node.docs.append(group.name);
        node.docs.append("/)\n:   ");
      } else if (token.substr(1) == "ingroup") {
        node.ingroup = consumeWord();

      /* legacy commands */
      } else if (token.substr(1) == "returns" ||
          token.substr(1) == "result") {
        node.docs.append(":material-location-exit: **Return**\n:   ");
      } else if (token.substr(1) == "sa") {
        node.docs.append(":material-eye-outline: **See**\n:   ");
      } else if (token.substr(1) == "file") {
        node.type = NodeType::FILE;
      } else if (token.substr(1) == "internal") {
        node.hide = true;
      } else if (token.substr(1) == "brief" ||
          token.substr(1) == "short") {
        node.brief.append(consumeSentence());
      } else if (token.substr(1) == "e" ||
          token.substr(1) == "em" ||
          token.substr(1) == "a") {
        node.docs.append("*");
        node.docs.append(consumeWord());
        node.docs.append("*");
      } else if (token.substr(1) == "b") {
        node.docs.append("**");
        node.docs.append(consumeWord());
        node.docs.append("**");
      } else if (token.substr(1) == "c") {
        node.docs.append("`");
        node.docs.append(consumeWord());
        node.docs.append("`");
      } else if (token.substr(1) == "f$") {
        node.docs.append("$");
      } else if (token.substr(1) == "f[" ||
          token.substr(1) == "f]") {
        node.docs.append("$$");
      } else if (token.substr(1) == "li" ||
          token.substr(1) == "arg") {
        node.docs.append("  - ");
      } else if (token.substr(1) == "ref") {
        auto href = consumeWord();
        auto text = consumeWord();
        node.docs.append("[");
        node.docs.append(text);
        node.docs.append("](#");
        node.docs.append(href);
        node.docs.append(")");
      } else if (token.substr(1) == "code" ||
          token.substr(1) == "endcode" ||
          token.substr(1) == "verbatim" ||
          token.substr(1) == "endverbatim") {
        node.docs.append("```");
      } else if (token.substr(1) == "attention") {
        node.docs.append("!!! warning \"Attention\"\n");
        indent += 4;
        node.docs.append(indent, ' ');
      } else if (token.substr(1) == "todo") {
        node.docs.append("!!! example \"To-do\"\n");
        indent += 4;
        node.docs.append(indent, ' ');
      } else if (token.substr(1) == "remark") {
        node.docs.append("!!! quote \"Remark\"\n");
        indent += 4;
        node.docs.append(indent, ' ');

      /* unrecognized commands */
      } else {
        warn("unrecognized command: " << token.str());
        node.docs.append(token.str());
      }
    } else if (token.type & DOC_ESCAPE) {
      node.docs.append(token.substr(1));
    } else if (token.type & DOC_PARA) {
      node.docs.append("\n\n");
      indent = std::max(indent - 4, 0);
    } else if (token.type & DOC_LINE) {
      node.docs.append("\n");
      node.docs.append(indent, ' ');
    } else {
      node.docs.append(token.str());
    }
    token = tokenizer.next();
  }

  return node;
}
