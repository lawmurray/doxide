#include "Driver.hpp"
#include "yaml/YAMLParser.hpp"
#include "parser/Parser.hpp"
#include "markdown/MarkdownGenerator.hpp"

namespace fs = std::filesystem;

Driver::Driver(int argc, char** argv) :
    title("Untitled"),
    output("docs") {
  /* read in config file first, so that command-line options can override */
  config();

  /* command-line options */
  enum {
    TITLE_ARG = 256,
    DESCRIPTION_ARG,
    OUTPUT_ARG
  };

  int c, option_index;
  option long_options[] = {
      { "title", required_argument, 0, TITLE_ARG },
      { "description", required_argument, 0, DESCRIPTION_ARG },
      { "output", required_argument, 0, OUTPUT_ARG },
      { 0, 0, 0, 0 }
  };
  const char* short_options = "-";  // treats non-options as short option 1

  opterr = 0;  // handle error reporting ourselves
  c = getopt_long_only(argc, argv, short_options, long_options, &option_index);
  while (c != -1) {
    switch (c) {
    case TITLE_ARG:
      title = optarg;
      break;
    case DESCRIPTION_ARG:
      description = optarg;
      break;
    case OUTPUT_ARG:
      output = optarg;
      break;
    case '?':  // unknown option
    case 1:  // not an option
      error("unrecognized option " << argv[optind - 1]);
      break;
    }
    c = getopt_long_only(argc, argv, short_options, long_options, &option_index);
  }

  /* some error checking */
  if (output.empty()) {
    error("--output cannot be empty");
  }
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

void Driver::help() {
  std::cout << "Usage:" << std::endl;
  std::cout << std::endl;
  std::cout << "doxide init --title Title --description Description" << std::endl;
  std::cout << std::endl;
  std::cout << "  Initialize the working directory for a new project." << std::endl;
  std::cout << std::endl;
  std::cout << "    --title (default Untitled): Title of the project." << std::endl;
  std::cout << "    --description (default empty): Description of the project." << std::endl;
  std::cout << std::endl;
  std::cout << "doxide build" << std::endl;
  std::cout << std::endl;
  std::cout << "   Build the project documentation." << std::endl;
  std::cout << std::endl;
  std::cout << "    --output (default docs): Output directory." << std::endl;
  std::cout << std::endl;
  std::cout << "doxide clean" << std::endl;
  std::cout << std::endl;
  std::cout << "    --output (default docs): Output directory." << std::endl;
  std::cout << std::endl;
  std::cout << "  Remove generated files." << std::endl;
  std::cout << std::endl;
  std::cout << "doxide help" << std::endl;
  std::cout << std::endl;
  std::cout << "  Print this help message." << std::endl;
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
        auto paths = glob(pattern);
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
