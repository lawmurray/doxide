#include "Driver.hpp"
#include "yaml/YAMLParser.hpp"
#include "cpp/CppParser.hpp"
#include "markdown/MarkdownGenerator.hpp"

namespace fs = std::filesystem;

static void write_file(const std::string& contents, const fs::path& dst) {
  std::fstream stream(dst, std::ios::out);
  stream << contents << '\n';
  stream.close();
}

static void write_file_prompt(const std::string& contents, const fs::path& dst) {
  if (fs::exists(dst)) {
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

static void copy_file_prompt(const fs::path& src, const fs::path& dst) {
  if (fs::exists(dst)) {
    std::cout << dst.string() << " already exists, overwrite? [y/N] ";
    std::string ans;
    std::getline(std::cin, ans);
    if (ans.length() > 0 && (ans[0] == 'y' || ans[0] == 'Y')) {
      fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
    }
  } else {
    fs::copy_file(src, dst, fs::copy_options::overwrite_existing);
  }
}

static std::string gulp(const fs::path& src) {
  std::string contents;
  std::ifstream in(src);
  char buffer[4096];
  while (in.read(buffer, sizeof(buffer))) {
    contents.append(buffer, sizeof(buffer));
  }
  contents.append(buffer, in.gcount());
  return contents;
}

static std::list<fs::path> glob(const std::string& pattern) {
  std::list<fs::path> results;
  glob_t matches;
  int rescode = glob(pattern.c_str(), 0, 0, &matches);
  if (rescode == 0) {
    for (int i = 0; i < (int)matches.gl_pathc; ++i) {
      results.push_back(matches.gl_pathv[i]);
    }
  }
  globfree(&matches);
  return results;
}

Driver::Driver(int argc, char** argv) :
    name("Untitled"),
    version("0.0.0"),
    output("docs") {
  /* command-line options */
  enum {
    NAME_ARG = 256,
    VERSION_ARG,
    DESCRIPTION_ARG,
    OUTPUT_ARG
  };

  int c, option_index;
  option long_options[] = {
      { "name", required_argument, 0, NAME_ARG },
      { "version", required_argument, 0, VERSION_ARG },
      { "description", required_argument, 0, DESCRIPTION_ARG },
      { "output", required_argument, 0, OUTPUT_ARG },
      { 0, 0, 0, 0 }
  };
  const char* short_options = "-";  // treats non-options as short option 1

  opterr = 0;  // handle error reporting ourselves
  c = getopt_long_only(argc, argv, short_options, long_options, &option_index);
  while (c != -1) {
    switch (c) {
    case NAME_ARG:
      name = optarg;
      break;
    case VERSION_ARG:
      version = optarg;
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
  std::stringstream stream;
  stream << "name: " << name << std::endl;
  stream << "version: " << version << std::endl;
  stream << "description: " << description << std::endl;
  stream << "files: " << std::endl;
  for (const auto& file : files) {
    stream << "  - " << file << std::endl;
  }
  write_file_prompt(stream.str(), "doxide.yaml");
}

void Driver::docs() {
  config();

  /* parse */
  CppParser parser;
  for (auto file: files) {
    parser.parse(gulp(file));
  }

  /* if a readme file exists, make it the front page; global entities are
   * appended to the page */
  if (fs::exists("README.md")) {
    fs::create_directories(output);
    fs::copy_file("README.md", fs::path(output) / "index.md",
        fs::copy_options::overwrite_existing);
  }

  /* generate */
  MarkdownGenerator generator;
  generator.generate(output, parser.root());
}

void Driver::clean() {
  /* traverse the output directory, removing any Markdown files with
   * 'generator: doxide' in their YAML frontmatter; these are files managed by
   * Doxide */
  for (auto& entry : fs::recursive_directory_iterator(output)) {
    if (entry.is_regular_file() && entry.path().extension() == ".md") {
      YAMLParser parser;
      parser.parse(gulp(entry.path()));
      auto& frontmatter = parser.root();
      if (frontmatter.isValue("generator") &&
          frontmatter.value("generator") == "doxide") {
        fs::remove(entry.path());
      }
    }
  }

  /* traverse the output directory again, this time removing any empty
   * directories; because removing a directory may make its parent directory
   * empty, repeat until there are no further empty directories */
  std::vector<fs::path> empty;
  do {
    empty.clear();
    for (auto& entry : fs::recursive_directory_iterator(output)) {
      if (entry.is_directory() && fs::is_empty(entry.path())) {
        empty.push_back(entry.path());
      }
    }
    for (auto& dir : empty) {
      fs::remove(dir);
    }    
  } while (empty.size());
}

void Driver::help() {
  std::cout << "Usage:" << std::endl;
  std::cout << std::endl;
  std::cout << "doxide init --name Name --version Version --description Description" << std::endl;
  std::cout << std::endl;
  std::cout << "  Initialize the working directory for a new project." << std::endl;
  std::cout << std::endl;
  std::cout << "    --name (default Untitled): Name of the project." << std::endl;
  std::cout << "    --version (default 0.0.0): Version of the project." << std::endl;
  std::cout << "    --description (default empty): Description of the project." << std::endl;
  std::cout << std::endl;
  std::cout << "doxide docs" << std::endl;
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
  fs::path path;
  if (std::filesystem::exists("doxide.yaml")) {
    path = "doxide.yaml";
  } else if (std::filesystem::exists("doxide.yml")) {
    path = "doxide.yml";
  } else if (std::filesystem::exists("doxide.json")) {
    path = "doxide.json";
  } else {
    std::cerr << "no doxide configuration file, use 'doxide init' to get set up." << std::endl;
    exit(EXIT_FAILURE);
  }

  /* parse build configuration file */
  YAMLParser parser;
  parser.parse(gulp(path));
  auto& config = parser.root();

  if (config.isValue("name")) {
    name = config.value("name");
  }
  if (config.isValue("version")) {
    version = config.value("version");
  }
  if (config.isValue("description")) {
    description = config.value("description");
  }

  /* expand file patterns in file list */
  files.clear();
  if (config.isSequence("files")) {
    for (auto& node : config.sequence("files")) {
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
}
