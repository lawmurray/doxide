#include "Driver.hpp"
#include "YAMLParser.hpp"
#include "Parser.hpp"
#include "MarkdownGenerator.hpp"
#include "GcovCounter.hpp"
#include "GcovGenerator.hpp"

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
  write_file_prompt(init_docs_javascripts_tablesort_js, "docs/javascripts/tablesort.js");
  write_file_prompt(init_docs_stylesheets_doxide_css, "docs/stylesheets/doxide.css");
  write_file_prompt(init_docs_overrides_partials_copyright_html, "docs/overrides/partials/copyright.html");
}

void Driver::build() {
  /* parse */
  parser.parse(filenames);

  /* incorporate coverage date */
  GcovCounter counter;
  counter.count(parser.root);

  /* generate */
  MarkdownGenerator generator(output);
  generator.generate(parser.root);
  generator.coverage(parser.root);
  generator.clean();
}

void Driver::cover() {
  /* parse */
  parser.parse(filenames);

  /* generate */
  GcovGenerator generator;
  generator.generate(parser.root);
}

void Driver::clean() {
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
  YAMLParser yaml(path);
  YAMLNode root = yaml.parse();

  if (root.has("title")) {
    if (root.isValue("title")) {
      title = root.value("title");
    } else {
      warn("'title' must be a value in configuration.");
    }
  }
  if (root.has("description")) {
    if (root.isValue("description")) {
      description = root.value("description");
    } else {
      warn("'description' must be a value in configuration.");
    }
  }
  if (root.has("output")) {
    if (root.isValue("output")) {
      output = root.value("output");
    } else {
      warn("'output' must be a value in configuration.");
    }
  }
  if (root.has("defines")) {
    if (root.isMapping("defines")) {
      const auto& map = root.mapping("defines");
      for (auto& [key, value] : map) {
        if (value->isValue()) {
          parser.defines[key] = value->value();
        }
      }
    } else {
      warn("'defines' must be a mapping in configuration.");
    }
  }
  
  /* expand file patterns in file list */
  filenames.clear();
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
          auto result = filenames.insert(path.string());
          if (!result.second) {
            warn("file " << path << " appears more than once in configuration.");
          }
        }
      }
    }
  }

  /* initialize root entity */
  groups(root, parser.root);
  parser.root.title = title;
  parser.root.docs = description;
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
