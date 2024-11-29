#pragma once

#include "doxide.hpp"

#include <filesystem>

/**
 * Source Watcher.
 *
 * Watches for changes of source files. The files to watch are specified by a glob pattern.
 *
 * @ingroup developer
 */
class SourceWatcher {
public:
  /**
   * Constructor for a list of patterns.
   *
   * @param patterns
   */
  SourceWatcher(std::list<std::string> patterns);

  /**
   * Constructor for a single pattern.
   */
  SourceWatcher(std::string pattern);

  /**
   * Check if any of the watched files changed since the last call to `changed()` or `diff()`.
   */
  bool changed();

  /**
   * Check if any of the watched files changed since the last call to `changed()` or `diff()`.
   *
   * @return A tuple containing sets of added, modified and deleted file paths.
   */
  std::tuple<std::unordered_set<std::filesystem::path>,
             std::unordered_set<std::filesystem::path>,
             std::unordered_set<std::filesystem::path>> diff();

  /**
   * The current source files.
   *
   */
  std::unordered_set<std::filesystem::path> filenames();

private:
  /**
   * The patterns to watch.
   */
  std::list<std::string> patterns;

  /**
   * The tracked file paths and their last modification time.
   */
  std::unordered_map<std::filesystem::path, std::filesystem::file_time_type> tracked_files;
};
