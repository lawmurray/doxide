#include "Entity.hpp"

Entity::Entity() :
   type(EntityType::NONE),
   hide(false) {
  //
}

void Entity::add(const Entity& node) {
  /* namespaces should not be added this way, instead retrieve with ns() */
  assert(node.type != EntityType::NAMESPACE);

  if (!node.hide && !node.docs.empty()) {
    if (!node.ingroup.empty() && node.ingroup != name) {
      group(node.ingroup).add(node);
    } else if (node.type == EntityType::TYPE) {
      types.insert({node.name, node});
    } else if (node.type == EntityType::VARIABLE) {
      variables.insert({node.name, node});
    } else if (node.type == EntityType::FUNCTION) {
      functions.insert({node.name, node});
    } else if (node.type == EntityType::OPERATOR) {
      operators.insert({node.name, node});
    } else if (node.type == EntityType::MACRO) {
      macros.insert({node.name, node});
    } else if (node.type == EntityType::ENUMERATOR) {
      enumerators.insert({node.name, node});
    } else if (node.type == EntityType::GROUP) {
      groups.insert({node.name, node});
    } else {
      warn("unrecognized node type, ignoring");
    }
  }
}

Entity& Entity::ns(const std::string& name) {
  auto iter = namespaces.find(name);
  if (iter != namespaces.end()) {
    return iter->second;
  } else {
    Entity node;
    node.type = EntityType::NAMESPACE;
    node.name = name;
    return namespaces.insert({name, node}).first->second;
  }
}

Entity& Entity::group(const std::string& name) {
  auto iter = groups.find(name);
  if (iter != groups.end()) {
    return iter->second;
  } else {
    Entity node;
    node.type = EntityType::GROUP;
    node.name = name;
    return groups.insert({name, node}).first->second;
  }
}
