#include "GcovCounter.hpp"
#include "YAMLParser.hpp"

void GcovCounter::count(const std::filesystem::path& coverage, Entity& root) {
  /*
  * A sketch of the format is:
  * ```
  *        -:    0:Source:/path/to/source/file.cpp
  *        -:    1:#include <iotream>>
  *        -:    4:
  *        -:    5:using namespace std;
  *        -:    6:
  *        1:    7:int main(int argc, char** argv) {
  *        1:    8:  if (true) {
  *        1:    9:    std::cout << "Hello, world!" << std::endl;
  *        -:   10:  } else {
  *    #####:   11:    std::cout << "Goodbye, world!" << std::endl;
  *        -:   12:  }
  *        1:   13:  return 0;
  *        -:   14:}
  * ```
  * The first column gives counts, `#####` indicating zero count on an
  * executable line. The second column gives line numbers, with the count
  * starting from one. Counts followed by `*` indicate an aggregate count over
  * function template instantiations; the function template instantiations
  * then follow, separated by lines of dashes, with disaggregated counts.
  * These disaggregated counts do not always seem complete, and so the
  * aggregated counts are used instead---the disagregated counts are readily
  * recognized and ignored due to the repetition of line numbers seen before.
  */
  static const std::regex regex_source("^\\s*-:\\s*0:Source:(.*)$",
      regex_flags);
  static const std::regex regex_covered("^\\s*(\\d+)\\*?:\\s*(\\d+):.*$",
      regex_flags);

  std::ifstream in(coverage);
  if (!in.is_open()) {
    throw std::runtime_error("could not read file " + coverage.string());
  }

  std::filesystem::path path;
  bool include = false;

  std::list<Entity*> es;
  Entity* file = nullptr;
  uint32_t nlines = 0;  // number of lines in file
  int max_line_number = 0;  // highest line number seen so far, used to
                                 // detect disaggreated counts
  std::string line;
  std::smatch match;

  while (std::getline(in, line)) {
    if (std::regex_match(line, match, regex_source)) {
      /* start of new file */
      path = match[1];
      if (path.is_absolute()) {
        path = std::filesystem::relative(path);
      }
      include = root.exists(path);
      if (include) {
        es = root.get(path);
        file = es.back();
        nlines = file->line_counts.size();
        max_line_number = 0;
      }
    } else if (include && std::regex_match(line, match, regex_covered)) {
      /* line data */
      int line_number = std::stoi(match[2]) - 1;
      int count = std::stoi(match[1]);
      if (line_number < 0 || line_number >= nlines) {
        warn("in " << file << ", " << path << ":" << line_number <<
            " does not exist; ignoring, are source and coverage" <<
            " files in sync?");
      } else if (count > 0 && line_number > max_line_number) {
        int& line_count = file->line_counts[line_number];
        if (line_count == 0) {
          /* line is included in report and first time seeing it covered,
           * update aggregate counts for whole path */
          for (auto e : es) {
            ++e->lines_covered;
          }
        }
        if (line_count >= 0) {
          /* line is included in report, update count */
          line_count += count;
        }
      }
      max_line_number = std::max(max_line_number, line_number);
    }
  }
}
