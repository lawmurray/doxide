#include "Driver.hpp"
#include "yaml/YAMLParser.hpp"
#include "parser/Parser.hpp"
#include "markdown/MarkdownGenerator.hpp"

Driver::Driver() :
    title("Untitled"),
    output("docs") {
  config();
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
  write_file_prompt(init_docs_stylesheets_doxide_css, "docs/stylesheets/doxide.css");
  write_file_prompt(init_docs_overrides_partials_copyright_html, "docs/overrides/partials/copyright.html");
}

void Driver::build() {
  clean();

  /* parse */
  Parser parser;
  for (auto file: files) {
    parser.parse(file, global);
  }

  /* generate */
  MarkdownGenerator generator;
  generator.generate(output, global);
}

void Driver::clean() {
  /* traverse the output directory, removing any Markdown files with
   * 'generator: doxide' in their YAML frontmatter; these are files managed by
   * Doxide */
  if (std::filesystem::exists(output) && std::filesystem::is_directory(output)) {
    for (auto& entry : std::filesystem::recursive_directory_iterator(output)) {
      if (entry.is_regular_file() && entry.path().extension() == ".md") {
        try {
          YAMLParser parser;
          YAMLNode frontmatter = parser.parse(entry.path());
          if (frontmatter.isValue("generator") &&
              frontmatter.value("generator") == "doxide") {
            std::filesystem::remove(entry.path());
          }
        } catch (const std::runtime_error& e) {
          // ignore
        }
      }
    }

    /* traverse the output directory again, this time removing any empty
    * directories; because removing a directory may make its parent directory
    * empty, repeat until there are no further empty directories */
    std::vector<std::filesystem::path> empty;
    do {
      empty.clear();
      for (auto& entry : std::filesystem::recursive_directory_iterator(output)) {
        if (entry.is_directory() && std::filesystem::is_empty(entry.path())) {
          empty.push_back(entry.path());
        }
      }
      for (auto& dir : empty) {
        std::filesystem::remove(dir);
      }    
    } while (empty.size());
  }
}

void Driver::config() {
  /* find the configuration file */
  std::filesystem::path path;
  if (std::filesystem::exists("doxide.yaml")) {
    path = "doxide.yaml";
  } else if (std::filesystem::exists("doxide.yml")) {
    path = "doxide.yml";
  } else if (std::filesystem::exists("doxide.json")) {
    path = "doxide.json";
  }

  /* parse build configuration file */
  YAMLParser parser;
  YAMLNode root = parser.parse(path);

  if (root.isValue("title")) {
    title = root.value("title");
  }
  if (root.isValue("description")) {
    description = root.value("description");
  }
  if (root.isValue("output")) {
    output = root.value("output");
  }

  /* expand file patterns in file list */
  files.clear();
  if (root.isSequence("files")) {
    for (auto& node : root.sequence("files")) {
      if (node->isValue()) {
        auto& pattern = node->value();
        auto paths = glob::rglob(pattern);
        if (paths.empty()) {
          /* print warning if pattern does not contain a wildcard '*' */
          if (pattern.find('*') == std::string::npos) {
            warn("no file matching '" + pattern + "' in configuration.");
          }
        } else for (auto path : paths) {
          auto result = files.insert(path.string());
          if (!result.second) {
            warn("file " << path << " appears more than once in configuration.");
          }
        }
      }
    }
  }

  /* initialize groups */
  groups(root, global);

  /* initialize meta data */
  global.title = title;
  global.docs = description;
}

void Driver::groups(YAMLNode& parentNode, Entity& parentEntity) {
  if (parentNode.isSequence("groups")) {
    for (auto& node : parentNode.sequence("groups")) {
      Entity entity;
      entity.type = EntityType::GROUP;
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
      parentEntity.add(entity);
    }
  }
}
