#pragma once

#include "DriverConfigParser.hpp"
#include "FileSystemDeleter.hpp"
#include "FileSystemStatsInfo.hpp"
#include "StyleOption.hpp"
#include "entities/EntityRegistry.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class YAMLNode;

/**
 * Driver for running commands
 *
 * @ingroup developer
 */
class Driver {
public:
  /**
   * Constructor.
   */
  Driver() = delete;

  Driver(const Driver&) = delete;

  Driver& operator=(const Driver&) = delete;

  Driver(const DriverInitInfo&, const std::optional<DriverConfigParser>&);

  /**
   * Create a new configuration file.
   */
  void init();

  /**
   * Build documentation.
   */
  void build();

  /**
   * Watch and build documentation on changes.
   */
  void watch();

  /**
   * Output line coverage information.
   */
  void cover();

  /**
   * Clean documentation.
   */
  void clean();

  /**
   * Markdown style.
   */
  StyleOption style;

  /**
   * Title.
   */
  std::string title;

  /**
   * Description.
   */
  std::string description;

  /**
   * Coverage file.
   */
  std::filesystem::path coverage;

  /**
   * Output directory.
   */
  std::filesystem::path output;
private:
  /**
   * Parse files.
   */
  void parse();

  /**
   * Count line coverage.
   */
  void count();

  void aggregate_stats();

  bool has_coverage() const noexcept;

  std::optional<std::filesystem::path> get_coverage_path() const;

  std::optional<std::string_view> get_coverage_suffix() const;

  void run_generation();

  void remove_file(const std::filesystem::path filename);

  /**
   * Read groups from the configuration file.
   */
  void groups(const YAMLNode& parentNode);

  FileSystemDeleter fscleaner;

  EntityRegistry registry;

  /**
   * Configuration file path.
   */
  std::optional<std::string> config_file;

  /**
   * Macro definitions.
   */
  std::unordered_map<std::string,std::string> defines;

  /**
   * Files.
   */
  std::unordered_set<std::filesystem::path> filenames;

  /**
   * Files patterns.
   */
  std::vector<std::string> files_patterns;

  FileSystemStatsInfo fsinfo;
};
