#include "FileSystemDeleter.hpp"

#include "YAMLNode.hpp"
#include "YAMLParser.hpp"
#include "Log.hpp"

#include <stdexcept>
#include <vector>

void FileSystemDeleter::delete_all_files() {
  if (std::filesystem::exists(output) && std::filesystem::is_directory(output)) {
    for (auto& entry : std::filesystem::recursive_directory_iterator(output)) {
      if (entry.is_regular_file() && entry.path().extension() == ".md" &&
          !files.contains(entry)) {
        try {
          YAMLParser parser;
          YAMLNode frontmatter = parser.parse(entry.path().string());
          if (frontmatter.isValue("generator") &&
              frontmatter.value("generator") == "doxide") {
            std::filesystem::remove(entry.path());
          }
        } catch (const std::runtime_error& e) {
          warn(e.what());
        }
      }
    }

    /* traverse the output directory again, this time removing any empty
     * directories; because removing a directory may make its parent directory
     * empty, repeat until there are no further empty directories */
    std::vector<std::filesystem::path> empty;
    do {
      empty.clear();
      for (auto& entry : std::filesystem::recursive_directory_iterator(output)) {
        if (entry.is_directory() && std::filesystem::is_empty(entry.path())) {
          empty.push_back(entry.path());
        }
      }
      for (auto& dir : empty) {
        std::filesystem::remove(dir);
      }
    } while (empty.size());
  }
}
