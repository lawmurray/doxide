const char* query_cpp = R""""(
[
  ;; namespace definition
  ((comment) @docs .
    (namespace_definition
      name: (namespace_identifier) @name
      body: (declaration_list)? @body) @namespace)
  ((namespace_definition
    name: (namespace_identifier) @name
    body: (declaration_list)? @body) @namespace)

  ;; nested namespace definition---matches once for each @name
  ((comment) @docs .
    (namespace_definition
      (nested_namespace_specifier
        (namespace_identifier) @name)
      body: (declaration_list)? @body) @namespace)
  ((namespace_definition
    (nested_namespace_specifier
      (namespace_identifier) @name)
    body: (declaration_list)? @body) @namespace)

  ;; class definition
  ((comment) @docs .
    (class_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)
  ((comment) @docs .
    (_ (class_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      )) @type)

  ;; struct definition
  ((comment) @docs .
    (struct_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)
  ((comment) @docs .
    (_ (struct_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      )) @type)

  ;; union definition
  ((comment) @docs .
    (union_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      ) @type)
  ((comment) @docs .
    (_ (union_specifier
      name: (type_identifier) @name
      body: (field_declaration_list)? @body
      )) @type)

  ;; enum definition
  ((comment) @docs .
    (enum_specifier
      name: (type_identifier) @name
      body: (enumerator_list)? @body
      ) @type)
  ((comment) @docs .
    (_ (enum_specifier
      name: (type_identifier) @name
      body: (enumerator_list)? @body
      )) @type)

  ;; class template definition
  ((comment) @docs .
    (template_declaration
      (class_specifier
        name: [(type_identifier) (template_type)] @name
        body: (field_declaration_list)? @body
        )) @type)
  ((comment) @docs .
    (_ (template_declaration
      (class_specifier
        name: [(type_identifier) (template_type)] @name
        body: (field_declaration_list)? @body
        ))) @type)

  ;; struct template definition
  ((comment) @docs .
    (template_declaration
      (struct_specifier
        name: [(type_identifier) (template_type)] @name
        body: (field_declaration_list)? @body
        )) @type)
  ((comment) @docs .
    (_ (template_declaration
      (struct_specifier
        name: [(type_identifier) (template_type)] @name
        body: (field_declaration_list)? @body
        ))) @type)

  ;; union template definition
  ((comment) @docs .
    (template_declaration
      (union_specifier
        name: [(type_identifier) (template_type)] @name
        body: (field_declaration_list)? @body
        )) @type)
  ((comment) @docs .
    (_ (template_declaration
      (union_specifier
        name: [(type_identifier) (template_type)] @name
        body: (field_declaration_list)? @body
        ))) @type)

  ;; typedef
  ((comment) @docs .
     (type_definition
       declarator: (type_identifier) @name .) @type)
  ((comment) @docs .
     (_ (type_definition
       declarator: (type_identifier) @name .)) @type)

  ;; type alias
  ((comment) @docs .
    (alias_declaration
      name: (type_identifier) @name) @type)
  ((comment) @docs .
    (_ (alias_declaration
      name: (type_identifier) @name)) @type)

  ;; type alias template
  ((comment) @docs .
    (template_declaration
      (alias_declaration
        name: (type_identifier) @name)) @type)
  ((comment) @docs .
    (_ (template_declaration
      (alias_declaration
        name: (type_identifier) @name))) @type)

  ;; concept
  ((comment) @docs .
    (template_declaration
      (concept_definition
        name: (identifier) @name
        (_))) @concept)
  ((comment) @docs .
    (_ (template_declaration
      (concept_definition
        name: (identifier) @name
        (_)))) @concept)

  ;; variable
  ((comment) @docs .
    (_
      declarator: [
        (identifier) @name
        (reference_declarator (identifier) @name)
        (pointer_declarator (identifier) @name)
        (field_identifier) @name
        (reference_declarator (field_identifier) @name)
        (pointer_declarator (field_identifier) @name)
      ]
      default_value: (_)? @value
    ) @variable
  )
  ((comment) @docs .
    (_
      (init_declarator
        declarator: [
          (identifier) @name
          (reference_declarator (identifier) @name)
          (pointer_declarator (identifier) @name)
          (field_identifier) @name
          (reference_declarator (field_identifier) @name)
          (pointer_declarator (field_identifier) @name)
        ]
        value: (_) @value
      )
    ) @variable
  )

  ;; function
  ((comment) @docs .
    (_
      declarator: (function_declarator
        declarator: [
          (identifier) @name
          (field_identifier) @name
          (destructor_name) @name
        ]
      )
      [
        (field_initializer_list)
        body: (_)
      ]? @body
    ) @function
  )
  ((comment) @docs .
    (_
      declarator: (_ (function_declarator
        ;; wildcard above is reference_declarator or pointer_declarator
        declarator: [
          (identifier) @name
          (field_identifier) @name
        ]
      ))
      [
        (field_initializer_list)
        body: (_)
      ]? @body
    ) @function
  )

  ;; function template
  ((comment) @docs .
    (template_declaration
      (_
        declarator: (function_declarator
          declarator: [
            (identifier) @name
            (field_identifier) @name
            (destructor_name) @name
          ]
        )
        [
          (field_initializer_list)
          body: (_)
        ]? @body
      )
    ) @function
  )
  ((comment) @docs .
    (template_declaration
      (_
        declarator: (_ (function_declarator
          ;; wildcard above is reference_declarator or pointer_declarator
          declarator: [
            (identifier) @name
            (field_identifier) @name
          ]
        ))
        [
          (field_initializer_list)
          body: (_)
        ]? @body
      )
    ) @function
  )

  ;; operator
  ((comment) @docs .
    (_
      declarator: (function_declarator
        declarator: (operator_name) @name
      )
      body: (_)? @body
    ) @operator
  )
  ((comment) @docs .
    (_
      declarator: (_ (function_declarator
        ;; wildcard above is reference_declarator or pointer_declarator
        declarator: (operator_name) @name
      ))
      body: (_)? @body
    ) @operator
  )

  ;; operator template
  ((comment) @docs .
    (template_declaration
      (_
        declarator: (function_declarator
          declarator: (operator_name) @name
        )
        body: (_)? @body
      )
    ) @operator
  )
  ((comment) @docs .
    (template_declaration
      (_
        declarator: (_ (function_declarator
          ;; wildcard above is reference_declarator or pointer_declarator
          declarator: (operator_name) @name
        ))
        body: (_)? @body
      )
    ) @operator
  )

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
