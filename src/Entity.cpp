#include "Entity.hpp"

Entity::Entity() :
   start_line(-1),
   end_line(-1),
   indent(0),
   type(EntityType::NONE),
   hide(false),
   visibleChildren(false) {
  //
}

void Entity::add(Entity&& o) {
  if (!o.ingroup.empty()) {
    if (addToGroup(std::move(o))) {
      return;
    } else {
      warn("ignoring @ingroup " << o.ingroup << ", no such group");
    }
  }
  addToThis(std::move(o));
}

bool Entity::addToGroup(Entity&& o) {
  /* search for immediate child of given name */
  auto iter = std::find_if(groups.begin(), groups.end(),
      [&o](const Entity& g) {
        return g.name == o.ingroup;
      });
  if (iter != groups.end()) {
    iter->addToThis(std::move(o));
    return true;
  }

  /* search recursively for descendent of given name */
  for (auto& g : groups) {
    if (g.addToGroup(std::move(o))) {
      return true;
    }
  }
  return false;
}

void Entity::addToThis(Entity&& o) {
  if (o.type == EntityType::NAMESPACE) {
    auto iter = std::find_if(namespaces.begin(), namespaces.end(),
        [&o](const Entity& ns) {
          return ns.name == o.name;
        });
    if (iter == namespaces.end()) {
      namespaces.push_back(std::move(o));
    } else {
      iter->merge(std::move(o));
    }
  } else if (o.type == EntityType::GROUP) {
    groups.push_back(std::move(o));
  } else if (o.type == EntityType::TYPE) {
    types.push_back(std::move(o));
  } else if (o.type == EntityType::CONCEPT) {
    concepts.push_back(std::move(o));
  } else if (o.type == EntityType::VARIABLE) {
    variables.push_back(std::move(o));
  } else if (o.type == EntityType::FUNCTION) {
    functions.push_back(std::move(o));
  } else if (o.type == EntityType::OPERATOR) {
    operators.push_back(std::move(o));
  } else if (o.type == EntityType::ENUMERATOR) {
    enums.push_back(std::move(o));
  } else if (o.type == EntityType::MACRO) {
    macros.push_back(std::move(o));
  } else if (o.type == EntityType::TEMPLATE) {
    // ignore, likely a parse error within the template declaration
  } else if (o.type == EntityType::FILE || o.type == EntityType::DIR) {
    /* maintain directory structure */
    std::filesystem::path path = o.name, partial;
    path = path.parent_path();
    Entity* e = this;
    for (auto iter = path.begin(); iter != path.end(); ++iter) {
      auto single = iter->string();
      auto found = std::find_if(e->files.begin(), e->files.end(),
          [&single](auto& e) { return e.name == single; });
      if (found == files.end()) {
        /* insert a subdirectory */
        partial /= single;
        e = &e->files.emplace_back();
        e->type = EntityType::DIR;
        e->name = partial;
        e->title = single;
      }
    }
    e->files.push_back(std::move(o));
  } else {
    warn("unrecognized entity type, ignoring");
  }
  visibleChildren = visibleChildren || !o.hide || !o.docs.empty();
}

void Entity::merge(Entity&& o) {
  /* add namespaces via the merging logic of addToThis() */
  std::for_each(o.namespaces.begin(), o.namespaces.end(),
      [this](Entity& o) {
        this->addToThis(std::move(o));
      });

  groups.splice(groups.end(), std::move(o.groups));
  types.splice(types.end(), std::move(o.types));
  concepts.splice(concepts.end(), std::move(o.concepts));
  variables.splice(variables.end(), std::move(o.variables));
  functions.splice(functions.end(), std::move(o.functions));
  operators.splice(operators.end(), std::move(o.operators));
  macros.splice(macros.end(), std::move(o.macros));
  enums.splice(enums.end(), std::move(o.enums));
  files.splice(enums.end(), std::move(o.files));

  if (ingroup.empty()) {
    ingroup = o.ingroup;
  }

  name = o.name;
  if (type == EntityType::TEMPLATE) {
    decl += " ";
    decl += o.decl;
  }
  docs += o.docs;
  brief += o.brief;
  type = o.type;
  hide = hide || o.hide;
  visibleChildren = visibleChildren || o.visibleChildren;
}
