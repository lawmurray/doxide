#include "Entity.hpp"

Entity::Entity() :
   start_line(0),
   end_line(0),
   lines_included(0),
   lines_covered(0),
   type(EntityType::ROOT),
   visible(false),
   hide(false) {
  //
}

void Entity::add(Entity&& o) {
  if (o.type == EntityType::NAMESPACE && !o.ingroup.empty()) {
    warn("namespace cannot have @ingroup, ignoring");
    o.ingroup.clear();
  }
  if (!o.ingroup.empty()) {
    if (addToGroup(std::move(o))) {
      return;
    } else {
      /* keep track of warnings and don't repeat them */
      static std::unordered_set<std::string> warned;
      if (warned.insert(o.ingroup).second) {
        warn("unrecognized group " << o.ingroup <<
            ", groups must be defined in config file, ignoring @ingroup");
      }
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
  } else if (o.type == EntityType::TYPEDEF) {
    typedefs.push_back(std::move(o));
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
    std::filesystem::path path = o.path, subdir;
    path = path.parent_path();
    Entity* e = this;
    e->lines_included += o.lines_included;
    e->lines_covered += o.lines_covered;
    for (auto iter = path.begin(); iter != path.end(); ++iter) {
      auto single = iter->string();
      auto found = std::find_if(e->dirs.begin(), e->dirs.end(),
          [&single](auto& s) { return s.name == single; });
      subdir /= single;
      if (found == e->dirs.end()) {
        /* add subdirectory */
        e = &e->dirs.emplace_back();
        e->type = EntityType::DIR;
        e->visible = true;
        e->name = single;
        e->path = subdir.string();
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
  visible = visible || o.visible;
}

void Entity::merge(Entity&& o) {
  /* add namespaces via the merging logic of addToThis() */
  std::for_each(o.namespaces.begin(), o.namespaces.end(),
      [this](Entity& o) {
        this->addToThis(std::move(o));
      });

  groups.splice(groups.end(), std::move(o.groups));
  types.splice(types.end(), std::move(o.types));
  typedefs.splice(types.end(), std::move(o.typedefs));
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
  end_line = uint32_t(std::max(line_counts.size(), o.line_counts.size()));
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
  visible = visible || o.visible;
}

bool Entity::exists(std::filesystem::path& path) const {
  auto parent_path = path.parent_path();
  auto e = this;
  for (auto iter = parent_path.begin(); iter != parent_path.end(); ++iter) {
    auto single = iter->string();
    auto found = std::find_if(e->dirs.begin(), e->dirs.end(),
        [&single](auto& s) { return s.name == single; });
    if (found == e->dirs.end()) {
      return false;
    } else {
      e = &(*found);
    }
  }

  auto file = path.filename().string();
  auto found = std::find_if(e->files.begin(), e->files.end(),
      [&file](auto& s) { return s.name == file; });
  return found != e->files.end();
}

std::list<Entity*> Entity::get(std::filesystem::path& path) {
  auto parent_path = path.parent_path();
  std::list<Entity*> r;
  auto e = this;
  r.push_back(e);
  for (auto iter = parent_path.begin(); iter != parent_path.end(); ++iter) {
    auto single = iter->string();
    auto found = std::find_if(e->dirs.begin(), e->dirs.end(),
        [&single](auto& s) { return s.name == single; });
    assert(found != e->dirs.end());
    e = &(*found);
    r.push_back(e);
  }

  auto file = path.filename().string();
  auto found = std::find_if(e->files.begin(), e->files.end(),
      [&file](auto& s) { return s.name == file; });
  assert(found != e->files.end());
  e = &(*found);
  r.push_back(e);

  return r;
}

void Entity::clear() {
  *this = Entity();
}

void Entity::delete_by_predicate(std::function<bool(const Entity&)> p){
	std::erase_if(namespaces, p);
	std::erase_if(groups, p);
	std::erase_if(types, p);
	std::erase_if(concepts, p);
	std::erase_if(variables, p);
	std::erase_if(functions, p);
	std::erase_if(operators, p);
	std::erase_if(enums, p);
	std::erase_if(macros, p);
	std::erase_if(dirs, p);
	std::erase_if(files, p);

	auto r = [p](Entity& e){ e.delete_by_predicate(p); };
        std::for_each(namespaces.begin(), namespaces.end(), r);
	std::for_each(groups.begin(), groups.end(), r);
	std::for_each(types.begin(), types.end(), r);
	std::for_each(concepts.begin(), concepts.end(), r);
        std::for_each(variables.begin(), variables.end(), r);
        std::for_each(functions.begin(), functions.end(), r);
        std::for_each(operators.begin(), operators.end(), r);
	std::for_each(enums.begin(), enums.end(), r);
	std::for_each(macros.begin(), macros.end(), r);
	std::for_each(dirs.begin(), dirs.end(), r);
	std::for_each(files.begin(), files.end(), r);
}
