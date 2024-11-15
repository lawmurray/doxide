#include "GcovGenerator.hpp"

void GcovGenerator::generate(const Entity& node) {
  /* collate coverage information */
  collate(node);

  /* JSON header */
  std::cout << "{" << std::endl;
  std::cout << "  \"format_version:\": \"2\"," << std::endl;
  std::cout << "  \"gcovr/format_version\": \"0.6\"," << std::endl;
  std::cout << "  \"files\": [" << std::endl;

  /* line coverage information */
  bool first_file = true;
  for (auto& [file, lines] : coverage) {
    if (!first_file) {
      std::cout << "," << std::endl;
    }
    first_file = false;
    std::cout << "    {" << std::endl;
    std::cout << "      \"file\": \"" << file << "\"," << std::endl;
    std::cout << "      \"functions\": []," << std::endl;
    std::cout << "      \"lines\": [" << std::endl;
    bool first_line = true;
    for (auto line : lines) {
      if (!first_line) {
        std::cout << "," << std::endl;
      }
      first_line = false;
      std::cout << "        {" << std::endl;
      std::cout << "          \"line_number\": " << (line + 1) << "," << std::endl;
      std::cout << "          \"branches\": []," << std::endl;
      std::cout << "          \"count\": 0" << std::endl;
      std::cout << "        }";
    }
    std::cout << std::endl;
    std::cout << "      ]" << std::endl;
    std::cout << "    }";
  }
  std::cout << std::endl;

  /* JSON footer */
  std::cout << "  ]" << std::endl;
  std::cout << "}" << std::endl;
}

void GcovGenerator::collate(const Entity& node) {
  /* record lines for functions and operators */
  if ((node.type == EntityType::FUNCTION || node.type == EntityType::OPERATOR)) {
    auto& lines = coverage[node.file];
    /* omit the first line (containing the opening brace) and the last line
     * (containing the closing brace); this is not perfect and misses function
     * bodies defined on a single line, although this is assumed to be rare */
    for (int line = node.middle_line + 1; line < node.end_line; ++line) {
      lines.insert(line);
    }
  }

  /* recurse into child entities that may be, or may contain, functions and
   * operators */
  for (auto& child : node.namespaces) {
    collate(child);
  }
  for (auto& child : node.groups) {
    collate(child);
  }
  for (auto& child : node.types) {
    collate(child);
  }
  for (auto& child : node.functions) {
    collate(child);
  }
  for (auto& child : node.operators) {
    collate(child);
  }
}