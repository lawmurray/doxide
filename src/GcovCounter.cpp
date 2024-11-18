#include "GcovCounter.hpp"
#include "YAMLParser.hpp"

void GcovCounter::count(Entity& root) {
  std::string gcov = "gcov.json";  ///@todo Allow configuration
  YAMLParser yaml(gcov, 0);
  YAMLNode node = yaml.parse();
  if (!node.isSequence("files")) {
    warn("missing 'files' key in " << gcov);
  } else for (auto file : node.sequence("files")) {
    if (!file->isValue("file")) {
      warn("missing 'file' key in 'files' element in " << gcov);
    } else if (!file->isSequence("lines")) {
      warn("missing 'lines' key in 'files' element in " << gcov);
    } else {
      auto filename = file->value("file");
      auto lines = file->sequence("lines");
      std::filesystem::path path(filename);
      if (path.is_absolute()) {
        /* make the path relative to the current working directory */
        path = std::filesystem::relative(path);
      }
      if (!root.exists(path)) {
        warn("file " << filename << " not found in documentation, ignoring");
      } else {
        /* update line coverage for this file */
        std::list<Entity*> es = root.get(path);
        Entity* file = es.back();
        for (auto line : lines) {
          if (!line->has("line_number")) {
            warn("missing 'line_number' key in 'lines' element in " << gcov);
          } else if (!line->has("count")) {
            warn("missing 'count' key in 'lines' element in " << gcov);
          } else {
            uint32_t line_number = std::stoi(line->value("line_number"));
            // ^ note line numbers start at 1 in this schema
            bool covered = std::stoi(line->value("count")) > 0;
            uint32_t nlines = file->line_counts.size();
            if (line_number > nlines) {
              warn("in " << gcov << ", " << filename << ":" << line_number <<
                  " does not exist; ignoring, are source and coverage" <<
                  " files in sync?");
            } else if (covered && file->line_counts[line_number - 1] >= 0) {
              /* line is included in report, update count */
              ++file->line_counts[line_number - 1];

              /* update aggregate counts for whole path */
              for (auto e : es) {
                ++e->lines_covered;
              }
            }
          }
        }
      }
    }
  }
}
