#pragma once

#include "doxide.hpp"
#include "Entity.hpp"
#include "yaml/YAMLNode.hpp"

/**
 * Driver.
 */
class Driver {
public:
  /**
   * Constructor.
   */
  Driver(int argc, char** argv);

  /**
   * Create a new configuration file.
   */
  void init();

  /**
   * Build documentation.
   */
  void build();

  /**
   * Clean documentation.
   */
  void clean();

  /**
   * Print help message.
   */
  void help();

private:
  /**
   * Read in the configuration file.
   */
  void config();

  /**
   * Recursively read groups from the configuration file.
   */
  void groups(YAMLNode& parentNode, Entity& parentEntity);

  /**
   * Title.
   */
  std::string title;

  /**
   * Description.
   */
  std::string description;

  /**
   * Output directory.
   */
  std::string output;

  /**
   * Lists of files from config.
   */
  std::unordered_set<std::string> files;

  /**
   * Global namespace.
   */
  Entity global;
};
