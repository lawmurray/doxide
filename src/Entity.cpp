#include "Entity.hpp"

Entity::Entity() :
   type(EntityType::NONE),
   hide(false) {
  //
}

bool Entity::empty() const {
  bool result = true;
  result = result && std::all_of(groups.begin(), groups.end(),
      [](const Entity& g) {
        return g.empty();
      });
  result = result && std::all_of(namespaces.begin(), namespaces.end(),
      [](const Entity& ns) {
        return ns.empty();
      });
  result = result && types.empty();
  result = result && variables.empty();
  result = result && functions.empty();
  result = result && operators.empty();
  result = result && enums.empty();
  result = result && macros.empty();
  return result;
}

void Entity::add(const Entity& o) {
  if (!o.ingroup.empty()) {
    if (addToGroup(o)) {
      return;
    } else {
      warn("ignoring @ingroup " << o.ingroup << ", no such group");
    }
  }
  addToThis(o);
}

bool Entity::addToGroup(const Entity& o) {
  /* search for immediate child of given name */
  auto iter = std::find_if(groups.begin(), groups.end(),
      [&o](const Entity& g) {
        return g.name == o.ingroup;
      });
  if (iter != groups.end()) {
    iter->addToThis(o);
    return true;
  }

  /* search recursively for descendent of given name */
  for (auto& g : groups) {
    if (g.addToGroup(o)) {
      return true;
    }
  }
  return false;
}

void Entity::addToThis(const Entity& o) {
  if (o.type == EntityType::NAMESPACE) {
    auto iter = std::find_if(namespaces.begin(), namespaces.end(),
        [&o](const Entity& ns) {
          return ns.name == o.name;
        });
    if (iter == namespaces.end()) {
      namespaces.push_back(o);
    } else {
      iter->merge(o);
    }
  } else if (o.type == EntityType::GROUP) {
    groups.push_back(o);
  } else if (!o.hide && !o.docs.empty()) {
    if (o.type == EntityType::TYPE) {
      types.push_back(o);
    } else if (o.type == EntityType::VARIABLE) {
      variables.push_back(o);
    } else if (o.type == EntityType::FUNCTION) {
      functions.push_back(o);
    } else if (o.type == EntityType::OPERATOR) {
      operators.push_back(o);
    } else if (o.type == EntityType::ENUMERATOR) {
      enums.push_back(o);
    } else if (o.type == EntityType::MACRO) {
      macros.push_back(o);
    } else {
      warn("unrecognized entity type, ignoring");
    }
  }
}

void Entity::merge(const Entity& o) {
  /* add namespaces via the merging logic of addToThis() */
  std::for_each(o.namespaces.begin(), o.namespaces.end(),
      [this](const Entity& o) {
        this->addToThis(o);
      });

  groups.insert(groups.end(), o.groups.begin(), o.groups.end());
  types.insert(types.end(), o.types.begin(), o.types.end());
  variables.insert(variables.end(), o.variables.begin(), o.variables.end());
  functions.insert(functions.end(), o.functions.begin(), o.functions.end());
  operators.insert(operators.end(), o.operators.begin(), o.operators.end());
  macros.insert(macros.end(), o.macros.begin(), o.macros.end());
  enums.insert(enums.end(), o.enums.begin(), o.enums.end());
  name = o.name;
  decl = o.decl;
  docs += o.docs;
  brief += o.brief;
  ingroup = o.ingroup;
  type = EntityType::NAMESPACE;
  hide = hide || o.hide;
}
