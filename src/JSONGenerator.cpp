#include "JSONGenerator.hpp"

void JSONGenerator::generate(const Entity& root) {
  /* JSON header */
  std::cout << "{" << std::endl;
  std::cout << "  \"format_version:\": \"2\"," << std::endl;
  std::cout << "  \"gcovr/format_version\": \"0.6\"," << std::endl;
  std::cout << "  \"files\": [" << std::endl;

  /* JSON body */
  generate(root, 0);

  /* JSON footer */
  std::cout << std::endl;
  std::cout << "  ]" << std::endl;
  std::cout << "}" << std::endl;
}

int JSONGenerator::generate(const Entity& root, int nfiles) {
  for (auto& dir : root.dirs) {
    nfiles = generate(dir, nfiles);
  }
  for (auto& file : root.files) {
    if (nfiles > 0) {
      std::cout << "," << std::endl;
    }      
    std::cout << "    {" << std::endl;
    std::cout << "      \"file\": \"" << file.name << "\"," << std::endl;
    std::cout << "      \"functions\": []," << std::endl;
    std::cout << "      \"lines\": [" << std::endl;

    /* line information; note lines are stored zero-based, but must be
      * output 1-based */
    uint32_t nlines = 0;
    for (uint32_t line = 0; line < file.line_counts.size(); ++line) {
      if (file.line_counts[line] >= 0) {
        if (nlines > 0) {
          std::cout << "," << std::endl;
        }
        ++nlines;
        std::cout << "        {" << std::endl;
        std::cout << "          \"line_number\": " << (line + 1) << "," << std::endl;
        std::cout << "          \"branches\": []," << std::endl;
        std::cout << "          \"count\": " << file.line_counts[line] << std::endl;
        std::cout << "        }";
      }
    }
    std::cout << std::endl;
    std::cout << "      ]" << std::endl;
    std::cout << "    }";
    ++nfiles;
  }
  return nfiles;
}
