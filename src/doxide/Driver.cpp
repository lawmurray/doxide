#include "doxide/Driver.hpp"
#include "doxide/Config.hpp"
#include "doxide/Tokenizer.hpp"
#include "doxide/Parser.hpp"
#include "doxide/Generator.hpp"

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
  Parser parser;
  for (auto file: files) {
    parser.parse(file);
  }

  /* generate */
  Generator generator;
  generator.generate(output, parser.root());

  /* index file */
  fs::path docs(output);
  if (fs::exists("README.md")) {
    copy("README.md", docs / "index.md");
  } else {
    std::fstream stream(docs / "index.md", std::ios::out);
    stream << description << '\n';
    stream.close();
  }

  /* other files */
  fs::create_directories(docs);
  fs::create_directories(docs / "types");
  fs::create_directories(docs / "variables");
  fs::create_directories(docs / "functions");
  fs::create_directories(docs / "operators");
  fs::create_directories(docs / "classes");
  fs::create_directories(docs / "structs");
}

void Driver::clean() {
  fs::remove_all(output);
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
  /* parse build configuration file */
  Config parser;
  auto config = parser.parse();
  if (!config["name"].empty()) {
    name = config["name"].front();
  }
  if (!config["version"].empty()) {
    version = config["version"].front();
  }
  if (!config["description"].empty()) {
    description = config["description"].front();
  }

  /* expand file patterns in file list */
  files.clear();
  for (auto pattern : config["files"]) {
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
