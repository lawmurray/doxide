#include "cpp/CppParser.hpp"

extern "C" const TSLanguage* tree_sitter_cpp();

static const char* query_cpp = R""""(
[
  ((comment)? @docs .
    (namespace_definition
      name: (namespace_identifier) @name
      body: (_)? @body) @namespace)
  ((comment)? @docs .
    (namespace_definition
      (nested_namespace_specifier
        (namespace_identifier)+ @name .)
      body: (_)? @body) @namespace)

  ((comment)? @docs .
    (class_specifier
      name: (type_identifier) @name
      body: (_)? @body) @type)
  ((comment)? @docs .
    (struct_specifier
      name: (type_identifier) @name
      body: (_)? @body) @type)
  ((comment)? @docs .
    (union_specifier
      name: (type_identifier) @name
      body: (_)? @body) @type)
  ((comment)? @docs .
    (enum_specifier
      name: (type_identifier) @name
      body: (_)? @body) @type)
  ((comment)? @docs .
    (alias_declaration
      name: (type_identifier) @name
      body: (_)? @body) @type)

  ((comment)? @docs .
    (template_declaration
      (class_specifier
        name: (type_identifier) @name
        body: (_)? @body)) @type)
  ((comment)? @docs .
    (template_declaration
      (struct_specifier
        name: (type_identifier) @name
        body: (_)? @body)) @type)
  ((comment)? @docs .
    (template_declaration
      (union_specifier
        name: (type_identifier) @name
        body: (_)? @body)) @type)
  ((comment)? @docs .
    (template_declaration
      (enum_specifier
        name: (type_identifier) @name
        body: (_)? @body)) @type)
  ((comment)? @docs .
    (template_declaration
      (alias_declaration
        name: (type_identifier) @name
        body: (_)? @body)) @type)

  ((comment)? @docs .
     (type_definition
       declarator: (type_identifier) @name) @type)

  ((comment)? @docs .
    (declaration
      declarator: (identifier) @name) @variable)
  ((comment)? @docs .
    (field_declaration
      declarator: (field_identifier) @name
      default_value: (_)? @value) @variable)
  ((comment)? @docs .
    (declaration
      (init_declarator
        declarator: (identifier) @name
        value: (_) @value)) @variable)

  ((comment)? @docs .
    (declaration
      declarator: (function_declarator
        declarator: (identifier) @name)) @function)
  ((comment)? @docs .
    (field_declaration
      declarator: (function_declarator
        declarator: (field_identifier) @name)) @function)
  ((comment)? @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (identifier) @name)
      body: (_) @body) @function)
  ((comment)? @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (field_identifier) @name)
      body: (_) @body) @function)

  ((comment)? @docs .
    (template_declaration
      (declaration
        declarator: (function_declarator
          declarator: (identifier) @name))) @function)
  ((comment)? @docs .
    (template_declaration
      (function_definition
        declarator: (function_declarator
          declarator: (identifier) @name)
        body: (_) @body)) @function)

  ((comment)? @docs .
    (declaration
      declarator: (function_declarator
        declarator: (operator_name) @name)) @operator)
  ((comment)? @docs .
    (field_declaration
      declarator: (function_declarator
        declarator: (operator_name) @name)) @operator)
  ((comment)? @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (operator_name) @name)
      body: (_) @body) @operator)

  ((comment)? @docs .
    (template_declaration
      (declaration
        declarator: (function_declarator
          declarator: (operator_name) @name))) @operator)
  ((comment)? @docs .
    (template_declaration
      (function_definition
        declarator: (function_declarator
          declarator: (operator_name) @name)
        body: (_) @body)) @operator)

  ((comment)? @docs .
     (enumerator
       name: (identifier) @name) @enumerator)

  ((comment)? @docs .
     (preproc_def
       name: (identifier) @name
       value: (_) @value) @macro)

  ((comment)? @docs .
     (preproc_function_def
       name: (identifier) @name
       value: (_) @value) @macro)

]
)"""";

CppParser::CppParser() :
    parser(nullptr),
    query(nullptr) {
  /* parser */
  parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_cpp());

  /* query */
  uint32_t error_offset;
  TSQueryError error_type;
  query = ts_query_new(tree_sitter_cpp(), query_cpp, strlen(query_cpp),
      &error_offset, &error_type);
  if (error_type != TSQueryErrorNone) {
    std::string_view from(query_cpp + error_offset, 40);
    error("invalid query starting " << from << "...");
  }
}

CppParser::~CppParser() {
  ts_query_delete(query);
  ts_parser_delete(parser);
}

void CppParser::parse(const std::string_view& source) {
  ts_parser_reset(parser);
  TSTree *tree = ts_parser_parse_string(parser, NULL, source.data(),
      source.size());
  TSNode node = ts_tree_root_node(tree);
  parseEntity(source, node, global);
}

void CppParser::parseEntity(const std::string_view& source,
    TSNode& parentNode, Entity& parentEntity) {
  TSQueryCursor* cursor = ts_query_cursor_new();
  TSQueryMatch match;
  ts_query_cursor_exec(cursor, query, parentNode);
  while (ts_query_cursor_next_match(cursor, &match)) {
    Entity entity;
    uint64_t start = 0, end = 0;
    for (uint16_t i = 0; i < match.capture_count; ++i) {
      TSNode node = match.captures[i].node;
      uint32_t id = match.captures[i].index; 
      uint32_t length = 0;
      uint32_t k = ts_node_start_byte(node);
      uint32_t l = ts_node_end_byte(node);
      const char* name = ts_query_capture_name_for_id(query, id, &length);

      if (strncmp(name, "docs", length) == 0) {
        translator.translate(source.substr(k, l - k), entity);
      } else if (strncmp(name, "name", length) == 0) {
        entity.name = source.substr(k, l - k);
      } else if (strncmp(name, "body", length) == 0) {
        end = ts_node_start_byte(node);
        parseEntity(source, node, entity);
      } else if (strncmp(name, "value", length) == 0) {
        end = ts_node_start_byte(node);
      } else if (strncmp(name, "namespace", length) == 0) {
        entity.type = EntityType::NAMESPACE;
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
      } else if (strncmp(name, "type", length) == 0) {
        entity.type = EntityType::TYPE;
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
      } else if (strncmp(name, "variable", length) == 0) {
        entity.type = EntityType::VARIABLE;
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
      } else if (strncmp(name, "function", length) == 0) {
        entity.type = EntityType::FUNCTION;
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
      } else if (strncmp(name, "operator", length) == 0) {
        entity.type = EntityType::OPERATOR;
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
      } else if (strncmp(name, "enumerator", length) == 0) {
        entity.type = EntityType::ENUMERATOR;
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
      } else if (strncmp(name, "macro", length) == 0) {
        entity.type = EntityType::MACRO;
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
      } else {
        warn("unrecognized match");
      }
    }
    entity.decl = source.substr(start, end - start);
    parentEntity.add(entity);
  }
  ts_query_cursor_delete(cursor);
}
