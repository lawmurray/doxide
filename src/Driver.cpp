#include "Driver.hpp"

#include "CppParser.hpp"
#include "FileSystemStatsInfo.hpp"
#include "GcovCounter.hpp"
#include "JSONCounter.hpp"
#include "JSONGenerator.hpp"
#include "YAMLNode.hpp"
#include "Log.hpp"
#include "SourceWatcher.hpp"
#include "Utils.hpp"
#include "entities/FileEntity.hpp"
#include "entities/FwdRef.hpp"
#include "entities/LineStats.hpp"
#include "generation/Dfs_traversal.hpp"
#include "generation/MarkdownGenerator.hpp"
#include "generation/MdCoverageGenerator.hpp"


#include <glob/glob.hpp>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <optional>
#include <regex>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <utility>

/**
 * Contents of initial `doxide.yaml` file.
 *
 * @ingroup developer
 */
static const char* init_doxide_yaml =
R""""(title:
description:
style:
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
  - https://cdn.jsdelivr.net/npm/mathjax@4/tex-mml-chtml.js
  - https://cdn.jsdelivr.net/npm/echarts@6.1.0/dist/echarts.min.js
  - https://cdn.jsdelivr.net/npm/tablesort@5.7.1/src/tablesort.min.js
  - https://cdn.jsdelivr.net/npm/tablesort@5.7.1/src/sorts/tablesort.number.min.js
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
  --md-admonition-icon--typedef: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M 13.43 7.11 L 13.18 10 H 17 V 12 H 13 L 12.56 17.07 A 1 1 0 0 0 15.287 17.261 L 17.028 17.48 A 1 1 0 0 1 10.57 16.89 L 11 12 H 8 V 10 H 11.17 L 11.44 6.93 Z"/></svg>');
  --md-admonition-icon--concept: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M3.75 3.5a.25.25 0 0 0-.25.25v2.062a.75.75 0 1 1-1.5 0V3.75C2 2.783 2.783 2 3.75 2h2.062a.75.75 0 1 1 0 1.5Zm13.688-.75a.75.75 0 0 1 .75-.75h2.062c.966 0 1.75.783 1.75 1.75v2.062a.75.75 0 1 1-1.5 0V3.75a.25.25 0 0 0-.25-.25h-2.062a.75.75 0 0 1-.75-.75ZM2.75 17.438a.75.75 0 0 1 .75.75v2.062c0 .138.112.25.25.25h2.062a.75.75 0 1 1 0 1.5H3.75A1.75 1.75 0 0 1 2 20.25v-2.062a.75.75 0 0 1 .75-.75Zm18.5 0a.75.75 0 0 1 .75.75v2.062A1.75 1.75 0 0 1 20.25 22h-2.062a.75.75 0 1 1 0-1.5h2.062a.25.25 0 0 0 .25-.25v-2.062a.75.75 0 0 1 .75-.75Zm-18.5-8.25a.75.75 0 0 1 .75.75v4.124a.75.75 0 1 1-1.5 0V9.938a.75.75 0 0 1 .75-.75ZM9.188 2.75a.75.75 0 0 1 .75-.75h4.124a.75.75 0 1 1 0 1.5H9.938a.75.75 0 0 1-.75-.75Zm0 18.5a.75.75 0 0 1 .75-.75h4.124a.75.75 0 1 1 0 1.5H9.938a.75.75 0 0 1-.75-.75ZM21.25 9.188a.75.75 0 0 1 .75.75v4.124a.75.75 0 1 1-1.5 0V9.938a.75.75 0 0 1 .75-.75ZM3.75 8.25a.75.75 0 0 1 .75-.75h2a.75.75 0 0 1 0 1.5h-2a.75.75 0 0 1-.75-.75Zm5.5 0A.75.75 0 0 1 10 7.5h2A.75.75 0 0 1 12 9h-2a.75.75 0 0 1-.75-.75Zm-1-4.5A.75.75 0 0 1 9 4.5v2a.75.75 0 0 1-1.5 0v-2a.75.75 0 0 1 .75-.75Zm0 5.5A.75.75 0 0 1 9 10v2a.75.75 0 0 1-1.5 0v-2a.75.75 0 0 1 .75-.75Zm0 4.75a.75.75 0 0 1 .75.75v4a.75.75 0 0 1-1.5 0v-4a.75.75 0 0 1 .75-.75ZM14 8.25a.75.75 0 0 1 .75-.75h4a.75.75 0 0 1 0 1.5h-4a.75.75 0 0 1-.75-.75Z"/></svg>');
  --md-admonition-icon--macro: url('data:image/svg+xml;charset=utf-8,<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="m5.41 21 .71-4h-4l.35-2h4l1.06-6h-4l.35-2h4l.71-4h2l-.71 4h6l.71-4h2l-.71 4h4l-.35 2h-4l-1.06 6h4l-.35 2h-4l-.71 4h-2l.71-4h-6l-.71 4h-2M9.53 9l-1.06 6h6l1.06-6h-6Z"/></svg>');
}

.md-typeset .admonition.variable, .md-typeset details.variable,
.md-typeset .admonition.function, .md-typeset details.function,
.md-typeset .admonition.typedef, .md-typeset details.typedef,
.md-typeset .admonition.concept, .md-typeset details.concept,
.md-typeset .admonition.macro, .md-typeset details.macro {
  border-color: var(--md-default-fg-color--lighter);
}

.md-typeset .variable > .admonition-title, .md-typeset .variable > summary,
.md-typeset .function > .admonition-title, .md-typeset .function > summary,
.md-typeset .typedef > .admonition-title, .md-typeset .typedef > summary,
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

.md-typeset .typedef > .admonition-title::before,
.md-typeset .typedef > summary::before {
  background-color: var(--md-default-fg-color--light);
  -webkit-mask-image: var(--md-admonition-icon--typedef);
          mask-image: var(--md-admonition-icon--typedef);
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

Driver::Driver(const DriverInitInfo& init_info, const std::optional<DriverConfigParser>& config_parser) :
  style(str_to_style_option(init_info.style)),
  title(init_info.title),
  description(init_info.description),
  coverage(init_info.coverage),
  output(init_info.output),
  fscleaner(output),
  registry(title, description) {
  if (config_parser) {
    config_file = config_parser->get_config_file();
    defines = config_parser->get_defines();
    std::tie(filenames, files_patterns) = config_parser->get_filenames_and_patterns();
    groups(config_parser->get_root());
  }
}

void Driver::init() {
  std::string doxide_yaml = init_doxide_yaml;
  std::string mkdocs_yaml = init_mkdocs_yaml;

  doxide_yaml = std::regex_replace(doxide_yaml, std::regex("title:"),
      "title: " + title);
  doxide_yaml = std::regex_replace(doxide_yaml, std::regex("description:"),
      "description: " + description);
  doxide_yaml = std::regex_replace(doxide_yaml, std::regex("style:"),
      std::format("style: {}", style_option_to_str(style)));

  mkdocs_yaml = std::regex_replace(mkdocs_yaml, std::regex("site_name:"),
      "site_name: " + title);
  mkdocs_yaml = std::regex_replace(mkdocs_yaml, std::regex("site_description:"),
      "site_description: " + description);

  write_file_prompt(doxide_yaml, "doxide.yaml");
  if (style == StyleOption::MKDOCS) {
    write_file_prompt(mkdocs_yaml, "mkdocs.yaml");
    write_file_prompt(init_docs_stylesheets_doxide_css, "docs/stylesheets/doxide.css");
    write_file_prompt(init_docs_overrides_partials_copyright_html, "docs/overrides/partials/copyright.html");
  }
}

void Driver::build() {
  parse();
  count();
  run_generation();
}

void Driver::watch() {
  build();

  std::cout << "Start watching" << std::endl;

  std::optional<SourceWatcher> config_watcher = config_file ?
    std::optional<SourceWatcher>{SourceWatcher(*config_file)} : std::optional<SourceWatcher>{};
  SourceWatcher watcher = SourceWatcher(files_patterns);

  CppParser parser(style);

  for (;;){
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    if (config_watcher and config_watcher->changed()) {
      std::cout << std::endl << "Detected configuration file change." << std::endl;
      std::cout << "Rebuilding documentation..." << std::endl;

      fscleaner.clear();
      fsinfo.clear();
      registry.clear();
      build();

      config_watcher = SourceWatcher(*config_file);
      watcher = SourceWatcher(files_patterns);

      std::cout << "Done" << std::endl;

      continue;
    }

    auto [added_files, changed_files, deleted_files] = watcher.diff();
    if (!added_files.empty() || !changed_files.empty() || !deleted_files.empty()){

      std::cout << std::endl << "Detected changes:" << std::endl;
      for (const auto& filename: added_files) {
        std::cout << "Added:   " << filename << std::endl;
      }
      for (const auto& filename: changed_files) {
        std::cout << "Changed: " << filename << std::endl;
      }
      for (const auto& filename: deleted_files) {
        std::cout << "Deleted: " << filename << std::endl;
      }
      std::cout << "Rebuilding documentation..." << std::endl;

      filenames = watcher.filenames();

      for (const auto& filename: deleted_files) {
        remove_file(filename);
      }
      for (const auto& filename: changed_files) {
        remove_file(filename);
      }

      for (const auto& filename: changed_files) {
        parser.parse(registry, filename, defines, has_coverage());
      }
      for (const auto& filename: added_files) {
        parser.parse(registry, filename, defines, has_coverage());
      }

      count();

      run_generation();

      std::cout << "Done" << std::endl;
    }
  }
}

void Driver::cover() {
  parse();
  count();

  JSONGenerator generator;
  generator.generate(fsinfo);
}

void Driver::clean() {
  /* can use MarkdownGenerator::clean() for this just by not calling
   * generate() first; this will remove all files with `generator: doxide` in
   * their YAML frontmatter */
  run_generation();
}

void Driver::parse() {
  CppParser parser(style);
  for (const auto& filename: filenames) {
    auto fp = filename.lexically_normal();
    if (fsinfo.contains(fp)) {
      throw std::runtime_error(std::format("file \"{}\" is already parsed", fp.native()));
    }
    std::optional<FwdRef<FileEntity>> file = parser.parse(registry, fp, defines, has_coverage());
    if (not file) {
      continue;
    }
    fsinfo.add_file(fp, *file);
  }
}

void Driver::count() {
  if (!coverage.empty()) {
    try {
      auto ext = coverage.extension();
      if (ext == ".gcov") {
        GcovCounter counter;
        counter.count(coverage, fsinfo);
      } else if (ext == ".json") {
        JSONCounter counter;
        counter.count(coverage, fsinfo);
      } else {
        throw std::runtime_error("unrecognized file extension on " +
            coverage.string());
      }
      aggregate_stats();
    } catch (const std::runtime_error& e) {
      warn(e.what() << ", coverage will be reported as zero");
    }
  }
}

void Driver::aggregate_stats() {
  DirStackController controller{fsinfo};
  for (auto& [path, dirinfo]: traverse_dfs_preorder<DirStackController, false>(controller)) {
    for(const FwdRef<FileEntity> fe: dirinfo.files) {
      dirinfo.stats += *fe;
    }
    for(const auto& sd: dirinfo.subdirs) {
      dirinfo.stats += fsinfo.lookup_dir(sd)->get().stats;
    }
  }
}

bool Driver::has_coverage() const noexcept {
  return not coverage.empty();
}

std::optional<std::filesystem::path> Driver::get_coverage_path() const {
  if (has_coverage()) {
    return output / "coverage";
  }
  return {};
}

std::optional<std::string_view> Driver::get_coverage_suffix() const {
  if (has_coverage()) {
    return "coverage";
  }
  return {};
}

void Driver::run_generation() {
  auto coverage_suffix = get_coverage_suffix();
  MarkdownGenerator generator(output, fscleaner, style);
  generator.generate(registry, coverage_suffix);
  if (auto coverage_path = get_coverage_path(); coverage_path) {
    MdCoverageGenerator cov_gen(*coverage_path, fscleaner);
    cov_gen.generate(fsinfo);
  }
  fscleaner.delete_all_files();
}

void Driver::remove_file(const std::filesystem::path filename) {
  fsinfo.remove_file(filename);
  registry.remove_file(filename);
}

void Driver::groups(const YAMLNode& groups_root) {
  if (not groups_root.isSequence("groups")) {
    return;
  }
  using parent_t = std::optional<FwdRef<GroupEntity>>;
  std::stack<std::tuple<const YAMLNode::sequence_type&, YAMLNode::sequence_type::const_iterator, parent_t>> stack;
  const auto& seq = groups_root.sequence("groups");
  stack.emplace(seq, seq.cbegin(), std::nullopt);

  while(not stack.empty()) {
    auto& [seq, iter, parent] = stack.top();
    if (iter == seq.cend()) {
      stack.pop();
      continue;
    }
    const auto& node = **iter;
    std::string_view name = node.isValue("name") ? std::string_view{node.value("name")} : "";
    std::string_view title = node.isValue("title") ? std::string_view{node.value("title")} : "";
    std::string_view desc = node.isValue("description") ? std::string_view{node.value("description")} : "";
    parent_t group = parent;
    if (name.size() == 0 or std::all_of(name.begin(), name.end(), isspace)) {
      // ignore the group
      warn("a group is missing a name in the configuration file.");
    } else {
      group = parent ? registry.add_group(*parent, name, title, desc)
        : registry.add_group(name, title, desc);
    }
    // move to next
    ++iter;
    if (node.isSequence("groups")) {
      // look for a subsequence and push
      const auto& child_seq = node.sequence("groups");
      stack.emplace(child_seq, child_seq.cbegin(), group);
    }
  }
}
