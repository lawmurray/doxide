#include "GcovGenerator.hpp"

void GcovGenerator::generate(const std::list<File>& files) {
  /* JSON header */
  std::cout << "{" << std::endl;
  std::cout << "  \"format_version:\": \"2\"," << std::endl;
  std::cout << "  \"gcovr/format_version\": \"0.6\"," << std::endl;
  std::cout << "  \"files\": [" << std::endl;

  /* line coverage information */
  bool first_file = true;
  for (const File& file : files) {
    if (!first_file) {
      std::cout << "," << std::endl;
    }
    first_file = false;
    std::cout << "    {" << std::endl;
    std::cout << "      \"file\": \"" << file.filename << "\"," << std::endl;
    std::cout << "      \"functions\": []," << std::endl;
    std::cout << "      \"lines\": [" << std::endl;
    bool first_line = true;
    for (uint32_t line : file.lines) {
      /* note: line numbers are zero-based, but JSON format expects 1-based */
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