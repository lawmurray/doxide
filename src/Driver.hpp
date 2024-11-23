#pragma once

#include "doxide.hpp"
#include "YAMLNode.hpp"
#include "Entity.hpp"

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
  Driver();

  /**
   * Create a new configuration file.
   */
  void init();

  /**
   * Build documentation.
   */
  void build();

  /**
   * Output line coverage information.
   */
  void cover();

  /**
   * Clean documentation.
   */
  void clean();

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
   * Read in the configuration file.
   */
  void config();

  /**
   * Parse files.
   */
  void parse();

  /**
   * Count line coverage.
   */
  void count();

  /**
   * Recursively read groups from the configuration file.
   */
  static void groups(YAMLNode& parentNode, Entity& parentEntity);

  /**
   * Root entity.
   */
  Entity root;

  /**
   * Macro definitions.
   */
  std::unordered_map<std::string,std::string> defines;

  /**
   * Files.
   */
  std::unordered_set<std::filesystem::path> filenames;
};
