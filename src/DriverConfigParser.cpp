#include "DriverConfigParser.hpp"

#include "Log.hpp"
#include "YAMLParser.hpp"

#include <glob/glob.hpp>

DriverConfigParser::DriverConfigParser(const std::string_view& _yaml_path):
  yaml_path(_yaml_path),
  root(YAMLParser().parse(yaml_path))
  {}

void DriverConfigParser::parse_init_info(DriverInitInfo& info) const {
  auto parse_value = [&](const std::string_view& value_name, std::string& out) {
    if (not out.empty()) {
      return;
    }
    if (root.has(value_name)) {
      if (root.isValue(value_name)) {
        out = root.value(value_name);
      } else {
        warn("'" << value_name << "' must be a value in configuration.");
      }
    }
  };
  parse_value("style", info.style);
  /* validation */
  if (info.style != "mkdocs" && info.style != "pandoc" && info.style != "plain") {
    warn("'style' must be either 'mkdocs', 'pandoc', or 'plain'. Using 'mkdocs'.");
    info.style = "mkdocs";
  }
  parse_value("title", info.title);
  parse_value("description", info.description);
  parse_value("coverage", info.coverage);
  parse_value("output", info.output);
}

std::unordered_map<std::string,std::string>
  DriverConfigParser::get_defines() const {
  if (not root.has("defines")) {
    return {};
  }
  if (not root.isMapping("defines")) {
    warn("'defines' must be a mapping in configuration.");
    return {};
  }
  std::unordered_map<std::string,std::string> defines;
  const auto& map = root.mapping("defines");
  for (auto& [key, value] : map) {
    if (value->isValue()) {
      defines[key] = value->value();
    }
  }
  return defines;
}

std::pair<std::unordered_set<std::filesystem::path>, std::vector<std::string>>
  DriverConfigParser::get_filenames_and_patterns() const {
  if (not root.isSequence("files")) {
    return std::pair<std::unordered_set<std::filesystem::path>, std::vector<std::string>>();
  }
  std::unordered_set<std::filesystem::path> filenames;
  std::vector<std::string> files_patterns;
  for (auto& node : root.sequence("files")) {
    if (node->isValue()) {
      auto& pattern = node->value();
      files_patterns.push_back(pattern);
      auto paths = glob::rglob(pattern);
      if (paths.empty()) {
        /* print warning if pattern does not contain a wildcard '*' */
        if (pattern.find('*') == std::string::npos) {
          warn("no file matching '" << pattern << "' in configuration.");
        }
      } else for (auto path : paths) {
        auto result = filenames.insert(path.string());
        if (!result.second) {
          warn("file " << path << " appears more than once in configuration.");
        }
      }
    }
  }
  return std::make_pair(filenames, files_patterns);
}
