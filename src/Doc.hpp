#pragma once

#include "doxide.hpp"

/**
 * Documentation of an entity.
 * 
 * @ingroup developer
 */
struct Doc {
  /**
   * Constructor.
   * 
   * @param comment Comment from which to populate documentation.
   */
  Doc(const std::string_view comment);

  /**
   * Content of the documentation.
   */
  std::string docs;

  /**
   * Group to which the entity belongs, obtained from @ingroup in the
   * documentation comment.
   */
  std::string ingroup;

  /**
   * Opening token, used to determine the type of comment.
   */
  DocToken open;

  /**
   * Hide the associated entity?
   */
  bool hide;
};
