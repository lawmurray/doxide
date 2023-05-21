#pragma once

#include "doxide.hpp"
#include "Entity.hpp"

/**
 * Documentation comment parser.
 */
class DocTranslator {
public:
  /**
   * Translate the documentation comment for an entity.
   * 
   * @param comment The comment.
   * @param[in,out] entity The entity.
   * 
   * Before return, sets the documentation comment of @p entity to the
   * translated comment, as well as any other attributes of @p entity as a
   * result of commands.
   */
 void translate(const std::string_view& comment, Entity& entity);
};
