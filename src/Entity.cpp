#include "Entity.hpp"

Entity::Entity() :
   type(EntityType::NONE),
   hide(false) {
  //
}

void Entity::add(const Entity& o) {
  if (!o.hide) {
    if (!o.ingroup.empty() && o.ingroup != name) {
      auto& group = groups[o.ingroup];
      group.name = o.ingroup;
      group.add(o);
    } else if (o.type == EntityType::NAMESPACE) {
      auto& ns = namespaces[o.name];
      ns.name = o.name;  // just created if did not already exist
      ns.merge(o);
    } else if (o.type == EntityType::GROUP) {
      auto& group = groups[o.name];
      group.name = o.name;  // just created if did not already exist
      group.merge(o);
    } else if (o.type == EntityType::TYPE) {
      types.insert({o.name, o});
    } else if (o.type == EntityType::VARIABLE) {
      variables.insert({o.name, o});
    } else if (o.type == EntityType::FUNCTION) {
      functions.insert({o.name, o});
    } else if (o.type == EntityType::OPERATOR) {
      operators.insert({o.name, o});
    } else if (o.type == EntityType::ENUMERATOR) {
      enumerators.insert({o.name, o});
    } else if (o.type == EntityType::MACRO) {
      macros.insert({o.name, o});
    } else {
      warn("unrecognized entity type, ignoring");
    }
  }
}

void Entity::merge(const Entity& o) {
  namespaces.insert(o.namespaces.begin(), o.namespaces.end());
  types.insert(o.types.begin(), o.types.end());
  variables.insert(o.variables.begin(), o.variables.end());
  functions.insert(o.functions.begin(), o.functions.end());
  operators.insert(o.operators.begin(), o.operators.end());
  macros.insert(o.macros.begin(), o.macros.end());
  enumerators.insert(o.enumerators.begin(), o.enumerators.end());
  groups.insert(o.groups.begin(), o.groups.end());
  name = o.name;
  decl = o.decl;
  docs += o.docs;
  brief += o.brief;
  ingroup = o.ingroup;
  type = EntityType::NAMESPACE;
  hide = hide || o.hide;
}
