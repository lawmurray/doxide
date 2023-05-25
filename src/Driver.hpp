#pragma once

#include "doxide.hpp"

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
   * Name.
   */
  std::string name;

  /**
   * Version.
   */
  std::string version;

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
};
