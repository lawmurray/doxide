const char* query_cpp = R""""(
[
  ;; documentation
  ((comment) @docs)

  ;; namespace definition
  ((namespace_definition
      name: (namespace_identifier) @name
      body: (declaration_list)? @body) @namespace)

  ;; nested namespace definition---matches once for each @name
  ((namespace_definition
      (nested_namespace_specifier
         (namespace_identifier) @name)
       body: (declaration_list)? @body) @namespace)

  ;; template declaration
  ((template_declaration
      (_) @body) @template)

  ;; class definition
  ((class_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)

  ;; struct definition
  ((struct_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)

  ;; union definition
  ((union_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)

  ;; enum definition
  ((enum_specifier
      name: (type_identifier) @name
      body: (enumerator_list)? @body
      ) @type)

  ;; typedef
  ((type_definition
       declarator: (type_identifier) @name .) @type)

  ;; type alias
  ((alias_declaration
      name: (type_identifier) @name) @type)

  ;; concept
  ((concept_definition
      name: (identifier) @name
      (_)) @concept)

  ;; variable
  ((declaration
      declarator: [
        (identifier) @name
        (reference_declarator (identifier) @name)
        (pointer_declarator (identifier) @name)
        (init_declarator
          declarator: [
            (identifier) @name
            (field_identifier) @name
            (reference_declarator (identifier) @name)
            (pointer_declarator (identifier) @name)
          ]
          value: (_) @value)
      ]
      default_value: (_)? @value
    ) @variable)

  ;; member variable
  ((field_declaration
      declarator: [
        (field_identifier) @name
        (reference_declarator (identifier) @name)
        (pointer_declarator (identifier) @name)
        (init_declarator
          declarator: [
            (identifier) @name
            (field_identifier) @name
            (reference_declarator (identifier) @name)
            (pointer_declarator (identifier) @name)
          ]
          value: (_) @value)
      ]
      default_value: (_)? @value
    ) @variable)

  ;; function
  ((_
      declarator: [
        (function_declarator
          declarator: [
            (identifier) @name
            (field_identifier) @name
            (destructor_name) @name
          ]
        )
        (reference_declarator
          (function_declarator
            declarator: [
              (identifier) @name
              (field_identifier) @name
              (destructor_name) @name
            ]
          )
        )
        (pointer_declarator
          (function_declarator
            declarator: [
              (identifier) @name
              (field_identifier) @name
              (destructor_name) @name
            ]
          )
        )
        ]
        [
          (field_initializer_list)
          body: (_)
        ]? @body
    ) @function)

  ;; operator
  ((_
      declarator: [
        (function_declarator
          declarator: (operator_name) @name
        )
        (reference_declarator
          (function_declarator
            declarator: (operator_name) @name
          )
        )
        (pointer_declarator
          (function_declarator
            declarator: (operator_name) @name
          )
        )
        ]
        body: (_)? @body
    ) @operator)

  ;; enumeration value
  ((enumerator
       name: (identifier) @name) @enumerator)

  ;; macro
  ((preproc_def
      name: (identifier) @name
      value: (_) @value) @macro)
  ((preproc_function_def
      name: (identifier) @name
      value: (_) @value) @macro)

]
)"""";
