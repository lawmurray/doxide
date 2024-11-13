#pragma once

#include "doxide.hpp"
#include "Entity.hpp"
#include "YAMLNode.hpp"

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
   * Output directory.
   */
  std::string output;

  /**
   * Defines.
   */
  std::unordered_map<std::string,std::string> defines;

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
   * Lists of files from config.
   */
  std::unordered_set<std::string> files;

  /**
   * Global namespace.
   */
  Entity global;
};
