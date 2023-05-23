#pragma once

#include <string>
#include <regex>
#include <stack>
#include <list>
#include <unordered_set>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <variant>

#include <cassert>

#include <getopt.h>
#include <glob.h>
#include <yaml.h>
#include <tree_sitter/api.h>

/**
 * Print a warning message.
 */
#define warn(msg) \
    std::cerr << "warning: " << msg << std::endl;

/**
 * Print an error message and exit.
 */
#define error(msg) \
    std::cerr << "error: " << msg << std::endl; \
    exit(EXIT_FAILURE);

/**
 * Contents of initial doxide.yaml
 */
static const char* init_doxide_yaml =
R""""(name:
version:
description:
files:
  - "*.hpp"
  - "*/*.hpp"
  - "*/*/*.hpp"
  - "*/*/*/*.hpp"
  - "*/*/*/*/*.hpp"
)"""";

/**
 * Contents of initial mkdocs.yaml
 */
static const char* init_mkdocs_yaml =
R""""(site_name:
site_description: 
site_url: https://example.com
repo_url: https://github.com/lawmurray/doxide
repo_name: lawmurray/doxide
theme:
  name: material
  custom_dir: overrides
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
  icon:
    repo: material/github

markdown_extensions:
  - def_list
  - attr_list
  - admonition
  - footnotes
  - pymdownx.arithmatex:
      generic: true
  - pymdownx.emoji:
      emoji_index: !!python/name:materialx.emoji.twemoji
      emoji_generator: !!python/name:materialx.emoji.to_svg
  - pymdownx.superfences:
      custom_fences:
        - name: mermaid
          class: mermaid
          format: !!python/name:pymdownx.superfences.fence_div_format
plugins:
  - search
  - social
extra_css:
  - stylesheets/doxide.css
  - https://unpkg.com/mermaid@8.5.1/dist/mermaid.css
extra_javascript:
  - javascripts/mathjax.js
  - https://polyfill.io/v3/polyfill.min.js?features=es6
  - https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js
  - https://unpkg.com/mermaid@8.5.1/dist/mermaid.min.js
)"""";

/**
 * Contents of initial docs/javascripts/mathjax.js
 */
static const char* init_docs_javascripts_mathjax_js =
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

/**
 * Contents of initial docs/stylesheets/doxide.css
 */
static const char* init_docs_stylesheets_doxide_css =
R""""(:root {
  --md-admonition-icon--variable: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M20.41 3c1.39 2.71 1.94 5.84 1.59 9-.2 3.16-1.3 6.29-3.17 9l-1.53-1c1.61-2.43 2.55-5.2 2.7-8 .34-2.8-.11-5.57-1.3-8l1.71-1M5.17 3 6.7 4C5.09 6.43 4.15 9.2 4 12c-.34 2.8.12 5.57 1.3 8l-1.69 1c-1.4-2.71-1.96-5.83-1.61-9 .2-3.16 1.3-6.29 3.17-9m6.91 7.68 2.32-3.23h2.53l-3.78 5 2.2 4.92h-2.26L11.71 14l-2.43 3.33H6.76l3.9-5.12-2.13-4.76h2.27l1.28 3.23Z"/></svg>');
  --md-admonition-icon--function: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M15.6 5.29c-1.1-.1-2.07.71-2.17 1.82L13.18 10H16v2h-3l-.44 5.07a3.986 3.986 0 0 1-4.33 3.63 4.007 4.007 0 0 1-3.06-1.87l1.5-1.5c.24.74.9 1.31 1.73 1.38 1.1.1 2.07-.71 2.17-1.82L11 12H8v-2h3.17l.27-3.07c.19-2.2 2.13-3.83 4.33-3.63 1.31.11 2.41.84 3.06 1.87l-1.5 1.5c-.24-.74-.9-1.31-1.73-1.38Z"/></svg>');
  --md-admonition-icon--macro: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="m5.41 21 .71-4h-4l.35-2h4l1.06-6h-4l.35-2h4l.71-4h2l-.71 4h6l.71-4h2l-.71 4h4l-.35 2h-4l-1.06 6h4l-.35 2h-4l-.71 4h-2l.71-4h-6l-.71 4h-2M9.53 9l-1.06 6h6l1.06-6h-6Z"/></svg>');
}

.md-typeset .admonition.variable, .md-typeset details.variable,
.md-typeset .admonition.function, .md-typeset details.function,
.md-typeset .admonition.macro, .md-typeset details.macro {
  border-color: var(--md-default-fg-color--lighter);
}

.md-typeset .variable > .admonition-title, .md-typeset .variable > summary,
.md-typeset .function > .admonition-title, .md-typeset .function > summary,
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

.md-typeset .macro > .admonition-title::before,
.md-typeset .macro > summary::before {
  background-color: var(--md-default-fg-color--light);
  -webkit-mask-image: var(--md-admonition-icon--macro);
          mask-image: var(--md-admonition-icon--macro);
}
)"""";

/**
 * Contents of initial overrides/partials/copyright.html
 */
static const char* init_overrides_partials_copyright_html =
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
