#include "GcovGenerator.hpp"

void GcovGenerator::generate(const Entity& root) {
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

int GcovGenerator::generate(const Entity& root, int nfiles) {
  /* line coverage information */
  for (auto& file : root.files) {
    if (file.type == EntityType::DIR) {
      nfiles = generate(file, nfiles);
    } else if (file.type == EntityType::FILE) {
      /* file information */
      if (nfiles > 0) {
        std::cout << "," << std::endl;
      }      
      std::cout << "    {" << std::endl;
      std::cout << "      \"file\": \"" << file.name << "\"," << std::endl;
      std::cout << "      \"functions\": []," << std::endl;
      std::cout << "      \"lines\": [" << std::endl;

      /* line information; note lines are stored zero-based, but must be
       * output 1-based */
      int nlines = 0;
      for (uint32_t line : file.lines) {
        if (nlines > 0) {
          std::cout << "," << std::endl;
        }
        ++nlines;
        std::cout << "        {" << std::endl;
        std::cout << "          \"line_number\": " << (line + 1) << "," << std::endl;
        std::cout << "          \"branches\": []," << std::endl;
        std::cout << "          \"count\": 0" << std::endl;
        std::cout << "        }";
      }
      std::cout << std::endl;
      std::cout << "      ]" << std::endl;
      std::cout << "    }";
      ++nfiles;
    }
  }
  return nfiles;
}