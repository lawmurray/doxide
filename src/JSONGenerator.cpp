#include "JSONGenerator.hpp"

#include "FileSystemStatsInfo.hpp"
#include "Sink.hpp"
#include "Utils.hpp"

#include <string_view>

constexpr std::string_view FRONT = R""""({{
  "format_version:": "2",
  "gcovr/format_version": "0.6",
  "files": [
)"""";

constexpr std::string_view END = R""""(
  ]
}}
)"""";

constexpr std::string_view FILE_FRONT =
R""""(    {{
    "file": "{}",
    "functions": [],
    "lines": [)"""";

constexpr std::string_view LINE_CONTENT =
R""""(
      {{
        "line_number": {},
        "branches": [],
        "count": {}
      }})"""";

constexpr std::string_view FILE_END =
R""""(
    ]
  }})"""";

void JSONGenerator::generate(const FileSystemStatsInfo& fsinfo) {
  sink_t sink;
  sink.println(FRONT);

  bool first = true;
  ConstDirStackController controller{fsinfo};
  for (const auto& [_, dirinfo]:
    dag_dfs_traverse<DfsTraversalType::POST>(controller)) {
    if (dirinfo.files.empty()) {
      continue;
    }
    for(const auto& fe: dirinfo.files) {
      if (not first) {
        sink.println(",");
      }
      first = false;

      sink.println(FILE_FRONT, fe->get_filename());
      bool line_first = true;
      const auto& line_counts = fe->get_line_counts();
      for(std::size_t line = 0; line < line_counts.size(); line++) {
        if (line_counts[line] < 0) {
          continue;
        }
        if (not line_first) {
          sink.println(",");
        }
        line_first = false;
        sink.print(LINE_CONTENT, line + 1, line_counts[line]);
      }
      sink.println(FILE_END);
    }
  }
  sink.println(END);
}
