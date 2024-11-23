#include "Driver.hpp"
#include "YAMLParser.hpp"
#include "CppParser.hpp"
#include "MarkdownGenerator.hpp"
#include "GcovCounter.hpp"
#include "JSONCounter.hpp"
#include "JSONGenerator.hpp"

/**
 * Contents of initial `doxide.yaml` file.
 * 
 * @ingroup developer
 */
static const char* init_doxide_yaml =
R""""(title:
description:
files:
  - "*.hpp"
  - "**/*.hpp"
  - "*.h"
  - "**/*.h"
)"""";

/**
 * Contents of initial `mkdocs.yaml` file.
 * 
 * @ingroup developer
 */
static const char* init_mkdocs_yaml =
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
  - https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js
  - https://cdn.jsdelivr.net/npm/tablesort@5.3.0/src/tablesort.min.js
  - https://cdn.jsdelivr.net/npm/tablesort@5.3.0/src/sorts/tablesort.number.js
  - javascripts/tablesort.js
)"""";

/**
 * Contents of initial `docs/javascripts/mathjax.js` file.
 * 
 * @ingroup developer
 */
static const char* init_docs_javascripts_mathjax_js =
R""""(window.MathJax = {
  tex: {
    inlineMath: [["\\(", "\\)"]],
    displayMath: [["\\[", "\\]"]],
    processEscapes: true,
    processEnvironments: true
  },
  options: {
    ignoreHtmlClass: ".*|",
    processHtmlClass: "arithmatex"
  }
};

document$.subscribe(() => { 
  MathJax.typesetPromise()
})
)"""";

/**
 * Contents of initial `docs/javascripts/tablesort.js` file.
 * 
 * @ingroup developer
 */
static const char* init_docs_javascripts_tablesort_js =
R""""(document$.subscribe(function() {
  var tables = document.querySelectorAll("article table:not([class])")
  tables.forEach(function(table) {
    new Tablesort(table)
  })
})
)"""";

/**
 * Contents of initial `docs/stylesheets/doxide.css` file.
 * 
 * @ingroup developer
 */
static const char* init_docs_stylesheets_doxide_css =
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

/**
 * Contents of initial `docs/overrides/partials/copyright.html` file.
 * 
 * @ingroup developer
 */
static const char* init_docs_overrides_partials_copyright_html =
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

Driver::Driver() :
    title("Untitled"),
    output("docs") {
  //
}

void Driver::init() {
  std::string doxide_yaml = init_doxide_yaml;
  std::string mkdocs_yaml = init_mkdocs_yaml;

  doxide_yaml = std::regex_replace(doxide_yaml, std::regex("title:"),
      "title: " + title);
  doxide_yaml = std::regex_replace(doxide_yaml, std::regex("description:"),
      "description: " + description);
  
  mkdocs_yaml = std::regex_replace(mkdocs_yaml, std::regex("site_name:"),
      "site_name: " + title);
  mkdocs_yaml = std::regex_replace(mkdocs_yaml, std::regex("site_description:"),
      "site_description: " + description);

  write_file_prompt(doxide_yaml, "doxide.yaml");
  write_file_prompt(mkdocs_yaml, "mkdocs.yaml");
  write_file_prompt(init_docs_javascripts_mathjax_js, "docs/javascripts/mathjax.js");
  write_file_prompt(init_docs_javascripts_tablesort_js, "docs/javascripts/tablesort.js");
  write_file_prompt(init_docs_stylesheets_doxide_css, "docs/stylesheets/doxide.css");
  write_file_prompt(init_docs_overrides_partials_copyright_html, "docs/overrides/partials/copyright.html");
}

void Driver::build() {
  config();
  parse();
  count();

  MarkdownGenerator generator(output);
  generator.generate(root);
  generator.coverage(root);
  generator.clean();
}

void Driver::cover() {
  config();
  parse();
  count();

  JSONGenerator generator;
  generator.generate(root);
}

void Driver::clean() {
  config();

  /* can use MarkdownGenerator::clean() for this just by not calling
   * generate() first; this will remove all files with `generator: doxide` in
   * their YAML frontmatter */
  MarkdownGenerator generator(output);
  generator.clean();
}

void Driver::config() {
  /* find the configuration file */
  std::string path;
  if (std::filesystem::exists("doxide.yaml")) {
    path = "doxide.yaml";
  } else if (std::filesystem::exists("doxide.yml")) {
    path = "doxide.yml";
  } else if (std::filesystem::exists("doxide.json")) {
    path = "doxide.json";
  }

  /* parse build configuration file */
  YAMLParser parser(path);
  YAMLNode yaml = parser.parse();

  if (yaml.has("title")) {
    if (yaml.isValue("title")) {
      title = yaml.value("title");
    } else {
      warn("'title' must be a value in configuration.");
    }
  }
  if (yaml.has("description")) {
    if (yaml.isValue("description")) {
      description = yaml.value("description");
    } else {
      warn("'description' must be a value in configuration.");
    }
  }
  if (yaml.has("coverage")) {
    if (yaml.isValue("coverage")) {
      coverage = yaml.value("coverage");
    } else {
      warn("'coverage' must be a value in configuration.");
    }
  }
  if (yaml.has("output")) {
    if (yaml.isValue("output")) {
      output = yaml.value("output");
    } else {
      warn("'output' must be a value in configuration.");
    }
  }
  if (yaml.has("defines")) {
    if (yaml.isMapping("defines")) {
      const auto& map = yaml.mapping("defines");
      for (auto& [key, value] : map) {
        if (value->isValue()) {
          defines[key] = value->value();
        }
      }
    } else {
      warn("'defines' must be a mapping in configuration.");
    }
  }
  
  /* expand file patterns in file list */
  filenames.clear();
  if (yaml.isSequence("files")) {
    for (auto& node : yaml.sequence("files")) {
      if (node->isValue()) {
        auto& pattern = node->value();
        auto paths = glob::rglob(pattern);
        if (paths.empty()) {
          /* print warning if pattern does not contain a wildcard '*' */
          if (pattern.find('*') == std::string::npos) {
            warn("no file matching '" + pattern + "' in configuration.");
          }
        } else for (auto path : paths) {
          auto result = filenames.insert(path.string());
          if (!result.second) {
            warn("file " << path << " appears more than once in configuration.");
          }
        }
      }
    }
  }

  /* initialize root entity */
  groups(yaml, root);
  root.title = title;
  root.docs = description;
}

void Driver::parse() {
  CppParser parser;
  for (const auto& filename: filenames) {
    parser.parse(filename, defines, root);
  }
}

void Driver::count() {
  if (!coverage.empty()) {
    auto ext = coverage.extension();
    if (ext == ".gcov") {
      GcovCounter counter;
      counter.count(coverage, root);
    } else if (ext == ".json") {
      JSONCounter counter;
      counter.count(coverage, root);
    }
  }
}

void Driver::groups(YAMLNode& parentNode, Entity& parentEntity) {
  if (parentNode.isSequence("groups")) {
    for (auto& node : parentNode.sequence("groups")) {
      Entity entity;
      entity.type = EntityType::GROUP;
      entity.visible = true;
      if (node->isValue("name")) {
        entity.name = node->value("name");
        if (node->isValue("title")) {
          entity.title = node->value("title");
        }
        if (node->isValue("description")) {
          entity.docs = node->value("description");
        }
      } else {
        warn("a group is missing a name in the configuration file.")
      }
      groups(*node, entity);
      parentEntity.add(std::move(entity));
    }
  }
}
