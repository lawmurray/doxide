#include "JSONCounter.hpp"
#include "YAMLParser.hpp"

void JSONCounter::count(const std::filesystem::path& coverage, Entity& root) {
  YAMLParser parser(coverage);
  YAMLNode node = parser.parse();
  if (!node.isSequence("files")) {
    warn("missing 'files' key in " << coverage);
  } else for (auto file : node.sequence("files")) {
    if (!file->isValue("file")) {
      warn("missing 'file' key in 'files' element in " << file);
    } else if (!file->isSequence("lines")) {
      warn("missing 'lines' key in 'files' element in " << file);
    } else {
      auto lines = file->sequence("lines");
      std::filesystem::path path = file->value("file");
      if (path.is_absolute()) {
        /* make the path relative to the current working directory */
        path = std::filesystem::relative(path);
      }
      if (root.exists(path)) {  // otherwise not included
        /* update line coverage for this file */
        std::list<Entity*> es = root.get(path);
        Entity* file = es.back();
        int nlines = file->line_counts.size();
        for (auto line : lines) {
          if (!line->has("line_number")) {
            warn("missing 'line_number' key in 'lines' element in " << file);
          } else if (!line->has("count")) {
            warn("missing 'count' key in 'lines' element in " << file);
          } else {
            int line_number = std::stoi(line->value("line_number")) - 1;
            int count = std::stoi(line->value("count"));
            if (line_number < 0 || line_number >= nlines) {
              warn("in " << file << ", " << path << ":" << line_number <<
                  " does not exist; ignoring, are source and coverage" <<
                  " files in sync?");
            } else if (count > 0) {
              int& line_count = file->line_counts[line_number];
              if (line_count == 0) {
                /* line is included in report and first time seeing it
                 * covered, update aggregate counts for whole path */
                for (auto e : es) {
                  ++e->lines_covered;
                }
              }
              if (line_count >= 0) {
                /* line is included in report, update count */
                line_count += count;
              }
            }
          }
        }
      }
    }
  }
}
