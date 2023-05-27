const char* query_cpp = R""""(
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
      body: (enumerator_list)? @body
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
        declarator: [(identifier) (field_identifier) (destructor_name)] @name)) @function)
  ((comment) @docs .
    (field_declaration
      declarator: (function_declarator
        declarator: (field_identifier) @name)) @function)
  ((comment) @docs .
    (field_declaration
      declarator: (function_declarator
        declarator: (destructor_name) @name)) @function)
  ((comment) @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (identifier) @name)
      ;; body or, in case of constructor, a field initializer list
      [(field_initializer_list) body: (_)] @body
      ) @function)
  ((comment) @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (field_identifier) @name)
      ;; body or, in case of constructor, a field initializer list
      [(field_initializer_list) body: (_)] @body
      ) @function)
  ((comment) @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (destructor_name) @name)
      ;; body or, in case of constructor, a field initializer list
      [(field_initializer_list) body: (_)] @body
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
        ;; body or, in case of constructor, a field initializer list
        [(field_initializer_list) body: (_)] @body
        )) @function)

  ;; operator
  ((comment) @docs .
    (declaration
      declarator: (function_declarator
        declarator: (operator_name) @name)) @operator)
  ((comment) @docs .
    (function_definition
      declarator: (function_declarator
        declarator: (operator_name) @name)
      body: (_) @body
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
        body: (_) @body
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
