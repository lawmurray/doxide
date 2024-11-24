#pragma once

#include "doxide.hpp"
#include "DocToken.hpp"

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
   * @param init_indent Initial indent level.
   */
  Doc(const std::string_view comment, const int init_indent);

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
   * Current indent level of the documentation comment for this entity. This
   * is used for tracking indenting across multiple end-of-line comments, for
   * example following a @@note command, each following line should be
   * indented until the end of the paragraph.
   */
  int indent;

  /**
   * Hide the associated entity?
   */
  bool hide;
};
