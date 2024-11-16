#include "Driver.hpp"

const char* init_doxide_yaml =
R""""(title:
description:
files:
  - "*.hpp"
  - "**/*.hpp"
  - "*.h"
  - "**/*.h"
)"""";

const char* init_mkdocs_yaml =
R""""(site_name:
site_description: 
theme:
  name: material
  custom_dir: docs/overrides
  features:
    - navigation.indexes
  palette:
    # Palette toggle for light mode
    - scheme: default
      primary: red
      accent: red
      toggle:
        icon: material/brightness-7 
        name: Switch to dark mode

    # Palette toggle for dark mode
    - scheme: slate
      primary: red
      accent: red
      toggle:
        icon: material/brightness-4
        name: Switch to light mode

markdown_extensions:
  - def_list
  - attr_list
  - admonition
  - pymdownx.details
  - pymdownx.superfences
  - pymdownx.arithmatex:
      generic: true
  - pymdownx.emoji:
      emoji_index: !!python/name:material.extensions.emoji.twemoji
      emoji_generator: !!python/name:material.extensions.emoji.to_svg
plugins:
  - search
extra_css:
  - stylesheets/doxide.css
extra_javascript:
  - javascripts/mathjax.js
  - https://polyfill.io/v3/polyfill.min.js?features=es6
  - https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js
)"""";

const char* init_docs_javascripts_mathjax_js =
R""""(<div class="md-copyright">
  {% if config.copyright %}
    <div class="md-copyright__highlight">
      {{ config.copyright }}
    </div>
  {% endif %}
  {% if not config.extra.generator == false %}
    Made with
    <a href="https://doxide.org" target="_blank" rel="noopener">
      Doxide
    </a>
    and
    <a href="https://squidfunk.github.io/mkdocs-material/" target="_blank" rel="noopener">
      Material for MkDocs
    </a>
  {% endif %}
</div>
)"""";

const char* init_docs_stylesheets_doxide_css =
R""""(:root {
  --md-admonition-icon--variable: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M20.41 3c1.39 2.71 1.94 5.84 1.59 9-.2 3.16-1.3 6.29-3.17 9l-1.53-1c1.61-2.43 2.55-5.2 2.7-8 .34-2.8-.11-5.57-1.3-8l1.71-1M5.17 3 6.7 4C5.09 6.43 4.15 9.2 4 12c-.34 2.8.12 5.57 1.3 8l-1.69 1c-1.4-2.71-1.96-5.83-1.61-9 .2-3.16 1.3-6.29 3.17-9m6.91 7.68 2.32-3.23h2.53l-3.78 5 2.2 4.92h-2.26L11.71 14l-2.43 3.33H6.76l3.9-5.12-2.13-4.76h2.27l1.28 3.23Z"/></svg>');
  --md-admonition-icon--function: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M15.6 5.29c-1.1-.1-2.07.71-2.17 1.82L13.18 10H16v2h-3l-.44 5.07a3.986 3.986 0 0 1-4.33 3.63 4.007 4.007 0 0 1-3.06-1.87l1.5-1.5c.24.74.9 1.31 1.73 1.38 1.1.1 2.07-.71 2.17-1.82L11 12H8v-2h3.17l.27-3.07c.19-2.2 2.13-3.83 4.33-3.63 1.31.11 2.41.84 3.06 1.87l-1.5 1.5c-.24-.74-.9-1.31-1.73-1.38Z"/></svg>');
  --md-admonition-icon--concept: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M3.75 3.5a.25.25 0 0 0-.25.25v2.062a.75.75 0 1 1-1.5 0V3.75C2 2.783 2.783 2 3.75 2h2.062a.75.75 0 1 1 0 1.5Zm13.688-.75a.75.75 0 0 1 .75-.75h2.062c.966 0 1.75.783 1.75 1.75v2.062a.75.75 0 1 1-1.5 0V3.75a.25.25 0 0 0-.25-.25h-2.062a.75.75 0 0 1-.75-.75ZM2.75 17.438a.75.75 0 0 1 .75.75v2.062c0 .138.112.25.25.25h2.062a.75.75 0 1 1 0 1.5H3.75A1.75 1.75 0 0 1 2 20.25v-2.062a.75.75 0 0 1 .75-.75Zm18.5 0a.75.75 0 0 1 .75.75v2.062A1.75 1.75 0 0 1 20.25 22h-2.062a.75.75 0 1 1 0-1.5h2.062a.25.25 0 0 0 .25-.25v-2.062a.75.75 0 0 1 .75-.75Zm-18.5-8.25a.75.75 0 0 1 .75.75v4.124a.75.75 0 1 1-1.5 0V9.938a.75.75 0 0 1 .75-.75ZM9.188 2.75a.75.75 0 0 1 .75-.75h4.124a.75.75 0 1 1 0 1.5H9.938a.75.75 0 0 1-.75-.75Zm0 18.5a.75.75 0 0 1 .75-.75h4.124a.75.75 0 1 1 0 1.5H9.938a.75.75 0 0 1-.75-.75ZM21.25 9.188a.75.75 0 0 1 .75.75v4.124a.75.75 0 1 1-1.5 0V9.938a.75.75 0 0 1 .75-.75ZM3.75 8.25a.75.75 0 0 1 .75-.75h2a.75.75 0 0 1 0 1.5h-2a.75.75 0 0 1-.75-.75Zm5.5 0A.75.75 0 0 1 10 7.5h2A.75.75 0 0 1 12 9h-2a.75.75 0 0 1-.75-.75Zm-1-4.5A.75.75 0 0 1 9 4.5v2a.75.75 0 0 1-1.5 0v-2a.75.75 0 0 1 .75-.75Zm0 5.5A.75.75 0 0 1 9 10v2a.75.75 0 0 1-1.5 0v-2a.75.75 0 0 1 .75-.75Zm0 4.75a.75.75 0 0 1 .75.75v4a.75.75 0 0 1-1.5 0v-4a.75.75 0 0 1 .75-.75ZM14 8.25a.75.75 0 0 1 .75-.75h4a.75.75 0 0 1 0 1.5h-4a.75.75 0 0 1-.75-.75Z"/></svg>');
  --md-admonition-icon--macro: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="m5.41 21 .71-4h-4l.35-2h4l1.06-6h-4l.35-2h4l.71-4h2l-.71 4h6l.71-4h2l-.71 4h4l-.35 2h-4l-1.06 6h4l-.35 2h-4l-.71 4h-2l.71-4h-6l-.71 4h-2M9.53 9l-1.06 6h6l1.06-6h-6Z"/></svg>');
}

.md-typeset .admonition.variable, .md-typeset details.variable,
.md-typeset .admonition.function, .md-typeset details.function,
.md-typeset .admonition.concept, .md-typeset details.concept,
.md-typeset .admonition.macro, .md-typeset details.macro {
  border-color: var(--md-default-fg-color--lighter);
}

.md-typeset .variable > .admonition-title, .md-typeset .variable > summary,
.md-typeset .function > .admonition-title, .md-typeset .function > summary,
.md-typeset .concept > .admonition-title, .md-typeset .concept > summary,
.md-typeset .macro > .admonition-title, .md-typeset .macro > summary {
  background-color: var(--md-default-bg-color);
}

.md-typeset .variable > .admonition-title::before,
.md-typeset .variable > summary::before {
  background-color: var(--md-default-fg-color--light);
  -webkit-mask-image: var(--md-admonition-icon--variable);
          mask-image: var(--md-admonition-icon--variable);
}

.md-typeset .function > .admonition-title::before,
.md-typeset .function > summary::before {
  background-color: var(--md-default-fg-color--light);
  -webkit-mask-image: var(--md-admonition-icon--function);
          mask-image: var(--md-admonition-icon--function);
}

.md-typeset .concept > .admonition-title::before,
.md-typeset .concept > summary::before {
  background-color: var(--md-default-fg-color--light);
  -webkit-mask-image: var(--md-admonition-icon--concept);
          mask-image: var(--md-admonition-icon--concept);
}

.md-typeset .macro > .admonition-title::before,
.md-typeset .macro > summary::before {
  background-color: var(--md-default-fg-color--light);
  -webkit-mask-image: var(--md-admonition-icon--macro);
          mask-image: var(--md-admonition-icon--macro);
}
)"""";

const char* init_docs_overrides_partials_copyright_html =
R""""(<div class="md-copyright">
  {% if config.copyright %}
    <div class="md-copyright__highlight">
      {{ config.copyright }}
    </div>
  {% endif %}
  {% if not config.extra.generator == false %}
    Made with
    <a href="https://doxide.org" target="_blank" rel="noopener">
      Doxide
    </a>
    and
    <a href="https://squidfunk.github.io/mkdocs-material/" target="_blank" rel="noopener">
      Material for MkDocs
    </a>
  {% endif %}
</div>
)"""";

const char* query_cpp = R""""(
[
  ;; documentation
  (comment) @docs

  ;; namespace definition
  (namespace_definition
      name: (namespace_identifier) @name
      body: (declaration_list)? @body) @namespace

  ;; nested namespace definition---matches once for each @name
  (namespace_definition
      (nested_namespace_specifier
         (namespace_identifier) @name)
       body: (declaration_list)? @body) @namespace

  ;; template declaration
  (template_declaration
      [
        (class_specifier)
        (struct_specifier)
        (union_specifier)
        (alias_declaration)
        (concept_definition)
        (declaration)
        (field_declaration)
        (function_definition)
      ] @body) @template

  ;; class definition
  (class_specifier
      name: [
        (type_identifier) @name
        (template_type) @name  ;; for template specialization
      ]
      body: (field_declaration_list)? @body
      ) @type

  ;; struct definition
  (struct_specifier
      name: [
        (type_identifier) @name
        (template_type) @name  ;; for template specialization
      ]
      body: (field_declaration_list)? @body
      ) @type

  ;; union definition
  (union_specifier
      name: [
        (type_identifier) @name
        (template_type) @name  ;; for template specialization
      ]
      body: (field_declaration_list)? @body
      ) @type

  ;; enum definition
  (enum_specifier
      name: (type_identifier) @name
      body: (enumerator_list)? @body
      ) @type

  ;; typedef
  (type_definition
       declarator: (type_identifier) @name .) @type

  ;; type alias
  (alias_declaration
      name: (type_identifier) @name) @type

  ;; concept
  (concept_definition
      name: (identifier) @name
      (_)) @concept

  ;; variable
  (declaration
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
    ) @variable

  ;; member variable
  (field_declaration
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
    ) @variable

  ;; function
  (_
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
    ) @function

  ;; operator
  (_
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
        (operator_cast
          type: (_) @name
        )
      ]
      body: (_)? @body
    ) @operator

  ;; enumeration value
  (enumerator
       name: (identifier) @name) @enumerator

  ;; macro
  (preproc_def
      name: (identifier) @name
      value: (_) @value) @macro
  (preproc_function_def
      name: (identifier) @name
      value: (_) @value) @macro

  ;; executable code for line counting
  ([
     (expression)
     (field_initializer)
  ]) @expression

  ;; compile-time executable code that is removed for line counting
  (requires_clause) @requires
  (if_statement
     condition: (_) @condition) @if
]
)"""";

void write_file(const std::string& contents,
    const std::filesystem::path& dst) {
  if (dst.has_parent_path()) {
    std::filesystem::create_directories(dst.parent_path());
  }
  std::fstream stream(dst, std::ios::out);
  stream << contents << '\n';
  stream.close();
}

void write_file_prompt(const std::string& contents,
    const std::filesystem::path& dst) {
  if (dst.has_parent_path()) {
    std::filesystem::create_directories(dst.parent_path());
  }
  if (std::filesystem::exists(dst)) {
    std::cout << dst.string() << " already exists, overwrite? [y/N] ";
    std::string ans;
    std::getline(std::cin, ans);
    if (ans.length() > 0 && (ans[0] == 'y' || ans[0] == 'Y')) {
      write_file(contents, dst);
    }
  } else {
    write_file(contents, dst);
  }
}

void copy_file_prompt(const std::filesystem::path& src,
    const std::filesystem::path& dst) {
  if (dst.has_parent_path()) {
    std::filesystem::create_directories(dst.parent_path());
  }
  if (std::filesystem::exists(dst)) {
    std::cout << dst.string() << " already exists, overwrite? [y/N] ";
    std::string ans;
    std::getline(std::cin, ans);
    if (ans.length() > 0 && (ans[0] == 'y' || ans[0] == 'Y')) {
      std::filesystem::copy_file(src, dst,
          std::filesystem::copy_options::overwrite_existing);
    }
  } else {
    std::filesystem::copy_file(src, dst,
        std::filesystem::copy_options::overwrite_existing);
  }
}

std::string gulp(const std::filesystem::path& src) {
  std::string contents;
  std::ifstream in(src);
  char buffer[4096];
  while (in.read(buffer, sizeof(buffer))) {
    contents.append(buffer, sizeof(buffer));
  }
  contents.append(buffer, in.gcount());
  return contents;
}

int main(int argc, char** argv) {
  Driver driver;
  CLI::App app{"Modern documentation for modern C++.\n"};
  app.get_formatter()->column_width(30);
  app.add_option("--title",
      driver.title,
      "Main page title.");
  app.add_option("--description",
      driver.description,
      "Main page description.");
  app.add_option("--output", driver.output,
      "Output directory.");
  app.add_subcommand("init",
      "Initialize configuration files.")->
      fallthrough()->
      callback([&]() { driver.init(); });
  app.add_subcommand("build",
      "Build documentation in output directory.")->
      fallthrough()->
      callback([&]() { driver.build(); });
  app.add_subcommand("clean",
      "Clean output directory.")->
      fallthrough()->
      callback([&]() { driver.clean(); });
  app.add_subcommand("cover",
      "Output (on stdout) zero-count line data for mixing with code coverage reports.")->
      fallthrough()->
      callback([&]() { driver.cover(); });
  app.require_subcommand(1);
  CLI11_PARSE(app, argc, argv);
}
