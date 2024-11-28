# Contributing to Doxide

Contributions to Doxide are welcome via a pull request to the [GitHub repository](https://github.com/lawmurray/doxide). Similarly, bug reports and feature suggestions can be made by [opening an issue](https://github.com/lawmurray/doxide/issues), and questions asked by [starting a discussion](https://github.com/lawmurray/doxide/discussions).

There are many ways to contribute code to Doxide. To do so you will need knowledge of C++, and may need to learn how to use [Tree-sitter](https://tree-sitter.github.io/tree-sitter/) to parse and query C++ source files. A brief how-to on the latter is given here .

When developing Doxide, you will need to build it from source, and rebuild it from source after making any changes. See the [README.md](https://github.com/lawmurray/doxide?tab=readme-ov-file#installation) file for instructions on working from source.

## Using Tree-sitter

!!! tip
    You can also use the online [playground](https://tree-sitter.github.io/tree-sitter/playground).

Install the `tree-sitter` command-line interface. This will depend on your operating system. It is widely available in Linux distributions. Once installed you may need to configure it for the first time:
```
tree-sitter init-config
```

There may not be a C++ parser installed by default, and at any rate Doxide is written to a specific version of a specific parser. That parser can be found in the `contrib/tree-sitter-cuda` directory, which is a submodule tracking the [tree-sitter-cuda](https://github.com/tree-sitter-grammars/tree-sitter-cuda) CUDA parser, which is based on the [tree-sitter-cpp](https://github.com/tree-sitter/tree-sitter-cpp) C++ parser, which is based on the [tree-sitter-c](https://github.com/tree-sitter/tree-sitter-c) C parser. CUDA is a superset of C++ is sort-of-a-superset of C and so Doxide currently uses the CUDA parser.

From within the `contrib/tree-sitter-cuda` directory, run, for example:
```
tree-sitter parse ../../demo/parsing.hpp
```
This will parse the given file and output the parse tree to the terminal.

## Fixing an issue: a walkthrough

This is a walkthrough of fixing issue [#50](https://github.com/lawmurray/doxide/issues/50) reported on the Doxide GitHub repository. The issue was that Doxide would miss variables that had an array type:
```cpp
/**
 * Test member variable of array type.
 */
int arr[10];
```
They would not be included in the documentation, and their documentation comment would be attributed to the next entity.

To reproduce the issue:

1. Add code such as the above to `demo/parsing.hpp` as a test case.

2. From the root directory of the code repository, build the Doxide documentation and publish it with MkDocs:
```
doxide build
mkdocs serve
```

3. Point your browser to `localhost:8000`. This is actually the whole Doxide website that you see at [doxide.org](https://doxide.org). Confirm that the bug is apparent by navigating to the *Demo > Parsing* section. (Of course you won't see issue [#50](https://github.com/lawmurray/doxide/issues/50) anymore---it has been fixed!)

To fix the problem, we can start by seeing how Tree-sitter parses the code. Back in the `contrib/tree-sitter-cuda` directory, put the above code in a file called `test.cpp` and run:
```
tree-sitter parse test.cpp
```
The output is:
```
(translation_unit [0, 0] - [4, 0]
  (comment [0, 0] - [2, 3])
  (declaration [3, 0] - [3, 12]
    type: (primitive_type [3, 0] - [3, 3])
    declarator: (array_declarator [3, 4] - [3, 11]
      declarator: (identifier [3, 4] - [3, 7])
      size: (number_literal [3, 8] - [3, 10]))))
```

!!! tip
    We could run `tree-sitter parse ../../demo/parsing.hpp` instead, but the parse tree is much larger, and it may be difficult to track down the specific problem that way.

Now, bring up the source file [src/CppParser.cpp](https://github.com/lawmurray/doxide/blob/main/src/CppParser.cpp), where Doxide's Tree-sitter queries are given as hard-coded strings in the `query_cpp` variable. These are patterns that match parse trees to select entities (e.g. types, variables, functions, macros, etc) from the source code to document. This section in particular plucks out variables and member variables:
```
;; variable
((declaration
    declarator: [
      (identifier) @name
      (reference_declarator (identifier) @name)
      (pointer_declarator (identifier) @name)
      (init_declarator
        declarator: [
          (identifier) @name
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
      (reference_declarator (field_identifier) @name)
      (pointer_declarator (field_identifier) @name)
      (init_declarator
        declarator: [
          (identifier) @name
          (field_identifier) @name
          (reference_declarator (identifier) @name)
          (reference_declarator (field_identifier) @name)
          (pointer_declarator (identifier) @name)
          (pointer_declarator (field_identifier) @name)
        ]
        value: (_) @value)
    ]
    default_value: (_)? @value
  ) @variable)
```

!!! tip
    For more information on writing queries, see the Tree-sitter documentation for [Pattern Matching with Queries](https://tree-sitter.github.io/tree-sitter/using-parsers#pattern-matching-with-queries).

The query handles, among other things, `reference_declarator` and `pointer_declarator`, but we saw `array_declarator` in the parse tree above, and it does not seem to handle that. This looks like it might be the problem. So let's augment the query to handle `array_declarator`, following the same patterns as for `reference_declarator` and `pointer_declarator`:
```
;; variable
((declaration
    declarator: [
      (identifier) @name
      (array_declarator (identifier) @name)
      (reference_declarator (identifier) @name)
      (pointer_declarator (identifier) @name)
      (init_declarator
        declarator: [
          (identifier) @name
          (array_declarator (identifier) @name)
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
      (array_declarator (field_identifier) @name)
      (reference_declarator (field_identifier) @name)
      (pointer_declarator (field_identifier) @name)
      (init_declarator
        declarator: [
          (identifier) @name
          (field_identifier) @name
          (array_declarator (identifier) @name)
          (array_declarator (field_identifier) @name)
          (reference_declarator (identifier) @name)
          (reference_declarator (field_identifier) @name)
          (pointer_declarator (identifier) @name)
          (pointer_declarator (field_identifier) @name)
        ]
        value: (_) @value)
    ]
    default_value: (_)? @value
  ) @variable)
```
Now rebuild and reinstall Doxide, rerun the test above (`doxide build`, `mkdocs serve`, inspect `localhost:8000`), and verify that the bug is now fixed.

You can now commit the fix, including the test case added to `parsing.hpp` as a regression test for future, and submit a pull request on the [GitHub repository](https://github.com/lawmurray/doxide).
