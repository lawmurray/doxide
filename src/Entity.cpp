#include "Entity.hpp"

Entity::Entity() :
   start_line(-1),
   end_line(-1),
   lines_included(0),
   lines_covered(0),
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
    std::filesystem::path path = o.filename;
    path = path.parent_path();
    Entity* e = this;
    for (auto iter = path.begin(); iter != path.end(); ++iter) {
      auto single = iter->string();
      auto found = std::find_if(e->dirs.begin(), e->dirs.end(),
          [&single](auto& s) { return s.name == single; });
      if (found == e->dirs.end()) {
        /* add subdirectory */
        e = &e->dirs.emplace_back();
        e->type = EntityType::DIR;
        e->name = single;
      } else {
        e = &(*found);
      }
      e->lines_included += o.lines_included;
      e->lines_covered += o.lines_covered;
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
  o.namespaces.clear();

  groups.splice(groups.end(), std::move(o.groups));
  types.splice(types.end(), std::move(o.types));
  concepts.splice(concepts.end(), std::move(o.concepts));
  variables.splice(variables.end(), std::move(o.variables));
  functions.splice(functions.end(), std::move(o.functions));
  operators.splice(operators.end(), std::move(o.operators));
  macros.splice(macros.end(), std::move(o.macros));
  enums.splice(enums.end(), std::move(o.enums));
  dirs.splice(dirs.end(), std::move(o.dirs));
  files.splice(files.end(), std::move(o.files));

  /* merge line counts, recalling that -1 indicates excluded lines, so cannot
   * simply add the two vectors */
  end_line = std::max(line_counts.size(), o.line_counts.size());
  line_counts.resize(end_line, -1);
  lines_included = 0;
  lines_covered = 0;
  for (uint32_t i = 0; i < end_line; ++i) {
    int count = (i < o.line_counts.size()) ? o.line_counts[i] : -1;
    if (line_counts[i] >= 0 || count >= 0) {
      line_counts[i] = std::max(line_counts[i], 0) + std::max(count, 0);
      ++lines_included;
      if (line_counts[i] > 0) {
        ++lines_covered;
      }
    }
  }

  if (ingroup.empty()) {
    ingroup = std::move(o.ingroup);
  }
  name = o.name;
  if (type == EntityType::TEMPLATE) {
    decl += " ";
    decl += std::move(o.decl);
  }
  docs += std::move(o.docs);
  brief += std::move(o.brief);
  type = o.type;
  hide = hide || o.hide;
  visibleChildren = visibleChildren || o.visibleChildren;
}
