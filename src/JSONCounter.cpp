#include "JSONCounter.hpp"
#include "YAMLParser.hpp"

void JSONCounter::count(const std::filesystem::path& file, Entity& root) {
  YAMLParser parser(file);
  YAMLNode node = parser.parse();
  if (!node.isSequence("files")) {
    warn("missing 'files' key in " << file);
  } else for (auto file : node.sequence("files")) {
    if (!file->isValue("file")) {
      warn("missing 'file' key in 'files' element in " << file);
    } else if (!file->isSequence("lines")) {
      warn("missing 'lines' key in 'files' element in " << file);
    } else {
      std::filesystem::path path = file->value("file");
      auto lines = file->sequence("lines");
      if (path.is_absolute()) {
        /* make the path relative to the current working directory */
        path = std::filesystem::relative(path);
      }
      if (root.exists(path)) {  // otherwise not included
        /* update line coverage for this file */
        std::list<Entity*> es = root.get(path);
        Entity* file = es.back();
        for (auto line : lines) {
          if (!line->has("line_number")) {
            warn("missing 'line_number' key in 'lines' element in " << file);
          } else if (!line->has("count")) {
            warn("missing 'count' key in 'lines' element in " << file);
          } else {
            uint32_t line_number = std::stoi(line->value("line_number"));
            // ^ note line numbers start at 1 in this schema
            bool covered = std::stoi(line->value("count")) > 0;
            uint32_t nlines = file->line_counts.size();
            if (line_number > nlines) {
              warn("in " << file << ", " << path << ":" << line_number <<
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
