#include "cpp/CppParser.hpp"

extern "C" const TSLanguage* tree_sitter_cpp();

static const char* query_cpp = R""""(
[
  ;; namespace definition
  ((comment)? @docs .
    (namespace_definition
      name: (namespace_identifier) @name
      body: (declaration_list)? @body) @namespace)

  ;; nested namespace definition---matches once for each @name
  ((comment)? @docs .
    (namespace_definition
      (nested_namespace_specifier
        (namespace_identifier) @name)
      body: (declaration_list)? @body) @namespace)

  ;; class definition
  ((comment) @docs .
    (class_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)

  ;; struct definition
  ((comment) @docs .
    (struct_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)

  ;; union definition
  ((comment) @docs .
    (union_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)

  ;; enum definition
  ((comment) @docs .
    (enum_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)

  ;; class template definition
  ((comment) @docs .
    (template_declaration
      (class_specifier
        name: (type_identifier) @name
        body: (field_declaration_list)? @body
        )) @type)

  ;; struct template definition
  ((comment) @docs .
    (template_declaration
      (struct_specifier
        name: (type_identifier) @name
        body: (field_declaration_list)? @body
        )) @type)

  ;; union template definition
  ((comment) @docs .
    (template_declaration
      (union_specifier
        name: (type_identifier) @name
        body: (field_declaration_list)? @body
        )) @type)

  ;; typedef
  ((comment) @docs .
     (type_definition
       declarator: (type_identifier) @name) @type)

  ;; type alias
  ((comment) @docs .
    (alias_declaration
      name: (type_identifier) @name) @type)

  ;; type alias template
  ((comment) @docs .
    (template_declaration
      (alias_declaration
        name: (type_identifier) @name)) @type)

  ;; variable
  ((comment) @docs .
    (declaration
      declarator: (identifier) @name) @variable)
  ((comment) @docs .
    (field_declaration
      declarator: (field_identifier) @name
      default_value: (_)? @value) @variable)
  ((comment) @docs .
    (declaration
      (init_declarator
        declarator: (identifier) @name
        value: (_) @value)) @variable)

  ;; function
  ((comment) @docs .
    (declaration
      declarator: (function_declarator
        declarator: (identifier) @name)) @function)
  ((comment) @docs .
    (field_declaration
      declarator: (function_declarator
        declarator: (field_identifier) @name)) @function)
  ((comment) @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (identifier) @name)
      ) @function)
  ((comment) @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (field_identifier) @name)
      ) @function)

  ;; function template
  ((comment) @docs .
    (template_declaration
      (declaration
        declarator: (function_declarator
          declarator: (identifier) @name))) @function)
  ((comment) @docs .
    (template_declaration
      (function_definition
        declarator: (function_declarator
          declarator: (identifier) @name)
        )) @function)

  ;; operator
  ((comment) @docs .
    (declaration
      declarator: (function_declarator
        declarator: (operator_name) @name)) @operator)
  ((comment) @docs .
    (field_declaration
      declarator: (function_declarator
        declarator: (operator_name) @name)) @operator)
  ((comment) @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (operator_name) @name)
      ) @operator)

  ;; operator template
  ((comment) @docs .
    (template_declaration
      (declaration
        declarator: (function_declarator
          declarator: (operator_name) @name))) @operator)
  ((comment) @docs .
    (template_declaration
      (function_definition
        declarator: (function_declarator
          declarator: (operator_name) @name)
        )) @operator)

  ;; enumeration value
  ((comment) @docs .
     (enumerator
       name: (identifier) @name) @enumerator)

  ;; macro
  ((comment) @docs .
     (preproc_def
       name: (identifier) @name
       value: (_) @value) @macro)
  ((comment) @docs .
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
  /* parser */
  ts_parser_reset(parser);
  TSTree *tree = ts_parser_parse_string(parser, NULL, source.data(),
      source.size());
  TSNode node = ts_tree_root_node(tree);

  /* initialize stacks */
  std::stack<uint32_t> starts, ends;
  std::stack<Entity> entities;
  starts.push(ts_node_start_byte(node));
  ends.push(ts_node_end_byte(node));
  entities.push(std::move(global));

  /* query */
  TSQueryCursor* cursor = ts_query_cursor_new();
  ts_query_cursor_exec(cursor, query, node);
  TSQueryMatch match;
  while (ts_query_cursor_next_match(cursor, &match)) {
    Entity entity;
    uint64_t start = 0, middle = 0, end = 0;
    for (uint16_t i = 0; i < match.capture_count; ++i) {
      node = match.captures[i].node;
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
        middle = ts_node_start_byte(node);
      } else if (strncmp(name, "value", length) == 0) {
        middle = ts_node_start_byte(node);
      } else {
        start = ts_node_start_byte(node);
        end = ts_node_end_byte(node);
        middle = end;

        if (strncmp(name, "namespace", length) == 0) {
          entity.type = EntityType::NAMESPACE;
        } else if (strncmp(name, "type", length) == 0) {
          entity.type = EntityType::TYPE;
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
        } else {
          warn("unrecognized match");
        }
      }
    }

    /* declaration only */
    entity.decl = source.substr(start, middle - start);

    /* this node represents the whole entity, pop the stack until we find its
     * direct parent, as indicated by nested byte ranges */
    while (!(starts.top() <= start && end <= ends.top())) {
      Entity top = std::move(entities.top());
      entities.pop();
      entities.top().add(top);
      starts.pop();
      ends.pop();
    }
    entities.push(std::move(entity));
    starts.push(start);
    ends.push(end);
  }

  /* finalize */
  while (entities.size() > 1) {
    Entity top = std::move(entities.top());
    entities.pop();
    entities.top().add(top);
  }
  global = std::move(entities.top());
  entities.pop();
  ts_query_cursor_delete(cursor);
}

void CppParser::parseEntity(const std::string_view& source,
    const TSQueryMatch& match) {
}
