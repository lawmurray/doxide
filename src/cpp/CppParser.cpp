#include "cpp/CppParser.hpp"

extern "C" const TSLanguage* tree_sitter_cpp();

void CppParser::parse(const std::string_view& source) {
  /* set up parser */
  TSParser* parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_cpp());
  TSTree *tree = ts_parser_parse_string(parser, NULL, source.data(),
      source.size());

  /* traverse syntax tree */
  TSTreeCursor cursor = ts_tree_cursor_new(ts_tree_root_node(tree));
  parseEntity(source, cursor, global);
}

void CppParser::parseEntity(const std::string_view& source,
    TSTreeCursor& cursor, Entity& parent) {
  TSNode node = ts_tree_cursor_current_node(&cursor);
  Entity entity;

  /* handle comments */
  while (strcmp(ts_node_type(node), "comment") == 0) {
    /* possibly a documentation comment */
    uint32_t i = ts_node_start_byte(node);
    uint32_t j = ts_node_end_byte(node);
    translator.translate(source.substr(i, j - i), entity);

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
            entity.type = EntityType::OPERATOR;
          } else {
            entity.type = EntityType::FUNCTION;
          }
          entity.name = source.substr(i, j - i);
          entity.decl = source.substr(k, l - k);
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

        entity.type = EntityType::TYPE;
        entity.name = source.substr(i, j - i);
        entity.decl = source.substr(k, l - k);

        if (ts_tree_cursor_goto_first_child(&cursor)) {
          parseEntity(source, cursor, entity);
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

        entity.type = EntityType::TYPE;
        entity.name = source.substr(i, j - i);
        entity.decl = source.substr(k, l - k);
      }
    }
  } else if (strcmp(ts_node_type(node), "namespace_definition") == 0) {
    /* namespace */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    if (!ts_node_is_null(name)) {
      Entity* ns = &parent;
      if (strcmp(ts_node_type(name), "namespace_definition_name") == 0) {
        /* nested namespaces */
        uint32_t n = ts_node_named_child_count(name);
        for (uint32_t i = 0; i < n; ++i) {
          TSNode child = ts_node_named_child(name, i);
          uint32_t k = ts_node_start_byte(child);
          uint32_t l = ts_node_end_byte(child);
          ns = &ns->ns(std::string(source.substr(k, l - k)));
        }
      } else {
        /* namespace */
        uint32_t i = ts_node_start_byte(name);
        uint32_t j = ts_node_end_byte(name);
        ns = &ns->ns(std::string(source.substr(i, j - i)));
      }
      if (ns->docs.empty()) {
        ns->docs = entity.docs;
      }
      if (ns->brief.empty()) {
        ns->brief = entity.docs;
      }
      if (ns->ingroup.empty()) {
        ns->ingroup = entity.ingroup;
      }
      ns->hide = ns->hide || entity.hide;
      if (ts_tree_cursor_goto_first_child(&cursor)) {
        parseEntity(source, cursor, *ns);
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

    entity.type = EntityType::TYPE;
    entity.name = source.substr(i, j - i);
    entity.decl = source.substr(k, l - k);

    if (ts_tree_cursor_goto_first_child(&cursor)) {
      parseEntity(source, cursor, entity);
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

      entity.type = EntityType::VARIABLE;
      entity.name = source.substr(i, j - i);
      entity.decl = source.substr(k, l - k);
    } else if (strcmp(ts_node_type(decl), "init_declarator") == 0) {
      /* global or namespace variable */
      TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

      uint32_t i = ts_node_start_byte(name);
      uint32_t j = ts_node_end_byte(name);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);

      entity.type = EntityType::VARIABLE;
      entity.name = source.substr(i, j - i);
      entity.decl = source.substr(k, l - k);
    } else if (strcmp(ts_node_type(decl), "function_declarator") == 0) {
      /* global or namespace function */
      TSNode name = ts_node_child_by_field_name(decl, "declarator", 10);

      uint32_t i = ts_node_start_byte(name);
      uint32_t j = ts_node_end_byte(name);
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(decl);

      if (strcmp(ts_node_type(name), "operator_name") == 0) {
        entity.type = EntityType::OPERATOR;
      } else {
        entity.type = EntityType::FUNCTION;
      }
      entity.name = source.substr(i, j - i);
      entity.decl = source.substr(k, l - k);
    }
  } else if (strcmp(ts_node_type(node), "field_declaration") == 0) {
    /* member variable */
    TSNode name = ts_node_child_by_field_name(node, "declarator", 10);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);

    entity.type = EntityType::VARIABLE;
    entity.name = source.substr(i, j - i);
    entity.decl = source.substr(k, l - k);
  } else if (strcmp(ts_node_type(node), "enumerator") == 0) {
    /* enumerator */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(node);

    entity.type = EntityType::ENUMERATOR;
    entity.name = source.substr(i, j - i);
    entity.decl = source.substr(k, l - k);
  } else if (strcmp(ts_node_type(node), "preproc_def") == 0) {
    /* macro */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = j;

    entity.type = EntityType::MACRO;
    entity.name = source.substr(i, j - i);
    entity.decl = source.substr(k, l - k);
  } else if (strcmp(ts_node_type(node), "preproc_function_def") == 0) {
    /* macro definition with arguments */
    TSNode name = ts_node_child_by_field_name(node, "name", 4);
    TSNode params = ts_node_child_by_field_name(node, "parameters", 10);

    uint32_t i = ts_node_start_byte(name);
    uint32_t j = ts_node_end_byte(name);
    uint32_t k = ts_node_start_byte(node);
    uint32_t l = ts_node_end_byte(params);

    entity.type = EntityType::MACRO;
    entity.name = source.substr(i, j - i);
    entity.decl = source.substr(k, l - k);
  } else {
    /* continue depth-first search */
    if (ts_tree_cursor_goto_first_child(&cursor)) {
      parseEntity(source, cursor, parent);
      ts_tree_cursor_goto_parent(&cursor);
    }
  }

  /* register node */
  if (entity.type != EntityType::NONE) {
    if (entity.ingroup.empty()) {
      parent.add(entity);
    } else {
      global.add(entity);
    }
  }

  /* next sibling */
  if (ts_tree_cursor_goto_next_sibling(&cursor)) {
    parseEntity(source, cursor, parent);
  }
}
