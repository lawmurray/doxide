#include "markdown/MarkdownGenerator.hpp"

void MarkdownGenerator::generate(const std::filesystem::path& dir,
    const Entity& entity) {
  std::ofstream out;
  std::filesystem::create_directories(dir / sanitize(entity.name));
  out.open(dir / sanitize(entity.name) / "index.md");
  out << frontmatter(entity) << std::endl;

  /* header */
  out << "# " << title(entity) << std::endl;
  out << std::endl;
  if (entity.type == EntityType::TYPE) {
    out << "**" << htmlize(line(entity.decl)) << "**" << std::endl;
    out << std::endl;
  }
  out << entity.docs << std::endl;
  out << std::endl;

  /* groups */
  for (auto& child : entity.groups) {
    out << ":material-format-section: [" << title(child) << ']';
    out << "(" << sanitize(child.name) << "/index.md)" << std::endl;
    out << ":   " << line(brief(child)) << std::endl;
    out << std::endl;
  }

  /* namespaces */
  for (auto& child : view(entity.namespaces, true)) {
    if (!child->empty()) {
      out << ":material-package: [" << child->name << ']';
      out << "(" << sanitize(child->name) << "/index.md)" << std::endl;
      out << ":   " << line(brief(*child)) << std::endl;
      out << std::endl;
    }
  }

  /* brief descriptions */
  if (entity.types.size() > 0) {
    out << "## Types" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& child : view(entity.types,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP)) {
      out << "| [" << child->name << "](" << sanitize(child->name) <<
          "/index.md) | ";
      out << line(brief(*child)) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.concepts.size() > 0) {
    out << "## Concepts" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& child : view(entity.concepts,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP)) {
      out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
      out << line(brief(*child)) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.macros.size() > 0) {
    out << "## Macros" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& child : view(entity.macros,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP)) {
      out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
      out << line(brief(*child)) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.variables.size() > 0) {
    out << "## Variables" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& child : view(entity.variables,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP)) {
      out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
      out << line(brief(*child)) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.operators.size() > 0) {
    out << "## Operators" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& child : view(entity.operators,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP)) {
      out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
      out << line(brief(*child)) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.functions.size() > 0) {
    out << "## Functions" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& child : view(entity.functions,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP)) {
      out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
      out << line(brief(*child)) << " |" << std::endl;
    }
    out << std::endl;
  }

  /* for an enumerator, output the possible values */
  if (entity.enums.size() > 0) {
    for (auto& child : entity.enums) {
      out << "**" << child.decl << "**" << std::endl;
      out << ":   " << child.docs << std::endl;
      out << std::endl;
    }
    out << std::endl;
  }

  /* detailed descriptions */
  if (entity.concepts.size() > 0) {
    out << "## Concept Details" << std::endl;
    out << std::endl;
    for (auto& child : view(entity.concepts, true)) {
      out << "### " << child->name;
      out << "<a name=\"" << sanitize(child->name) << "\"></a>" << std::endl;
      out << std::endl;
      out << "!!! concept \"" << htmlize(line(child->decl)) << '"' << std::endl;
      out << std::endl;
      out << indent(child->docs) << std::endl;
      out << std::endl;
    }
  }
  if (entity.macros.size() > 0) {
    out << "## Macro Details" << std::endl;
    out << std::endl;
    for (auto& child : view(entity.macros, true)) {
      out << "### " << child->name;
      out << "<a name=\"" << sanitize(child->name) << "\"></a>" << std::endl;
      out << std::endl;
      out << "!!! macro \"" << htmlize(line(child->decl)) << '"' << std::endl;
      out << std::endl;
      out << indent(child->docs) << std::endl;
      out << std::endl;
    }
  }
  if (entity.variables.size() > 0) {
    out << "## Variable Details" << std::endl;
    out << std::endl;
    for (auto& child : view(entity.variables, true)) {
      out << "### " << child->name;
      out << "<a name=\"" << sanitize(child->name) << "\"></a>" << std::endl;
      out << std::endl;
      out << "!!! variable \"" << htmlize(line(child->decl)) << '"' << std::endl;
      out << std::endl;
      out << indent(child->docs) << std::endl;
      out << std::endl;
    }
  }
  if (entity.operators.size() > 0) {
    out << "## Operator Details" << std::endl;
    out << std::endl;
    std::string prev;
    for (auto& child : view(entity.operators, true)) {
      if (child->name != prev) {
        /* heading only for the first overload of this name */
        out << "### " << child->name;
        out << "<a name=\"" << sanitize(child->name) << "\"></a>" << std::endl;
        out << std::endl;
      }
      out << "!!! function \"" << htmlize(line(child->decl)) << '"' << std::endl;
      out << std::endl;
      out << indent(child->docs) << std::endl;
      out << std::endl;
      prev = child->name;
    }
  }
  if (entity.functions.size() > 0) {
    out << "## Function Details" << std::endl;
    out << std::endl;
    std::string prev;
    for (auto& child : view(entity.functions, true)) {
      if (child->name != prev) {
        /* heading only for the first overload of this name */
        out << "### " << child->name;
        out << "<a name=\"" << sanitize(child->name) << "\"></a>" << std::endl;
      }
      out << "!!! function \"" << htmlize(line(child->decl)) << '"' << std::endl;
      out << std::endl;
      out << indent(child->docs) << std::endl;
      out << std::endl;
      prev = child->name;
    }
  }

  /* child pages */
  for (auto& child : entity.groups) {
    generate(dir / sanitize(entity.name), child);
  }
  for (auto& child : entity.namespaces) {
    if (!child.empty()) {
      generate(dir / sanitize(entity.name), child);
    }
  }
  for (auto& child : entity.types) {
    generate(dir / sanitize(entity.name), child);
  }
}

std::string MarkdownGenerator::frontmatter(const Entity& entity) {
  /* use YAML frontmatter to ensure correct capitalization of title, and to
   * mark as managed by Doxide */
  std::stringstream buf;
  buf << "title: " << title(entity) << std::endl;
  buf << "description: " << line(brief(entity)) << std::endl;
  buf << "generator: doxide" << std::endl;
  buf << "---" << std::endl;
  buf << std::endl;
  return buf.str();
}

std::string MarkdownGenerator::title(const Entity& entity) {
  if (!entity.title.empty()) {
    return entity.title;
  } else {
    return entity.name;
  }
}

std::string MarkdownGenerator::brief(const Entity& entity) {
  if (!entity.brief.empty()) {
    return entity.brief;
  } else {
    static const std::regex reg("^[^;:.?!]*?[\\.\\?\\!]");
    std::string l = line(entity.docs);
    std::smatch match;
    if (std::regex_search(l, match, reg)) {
      return match.str();
    } else {
      return l;
    }
  }
}

std::string MarkdownGenerator::line(const std::string& str) {
  static const std::regex newline("\\s*\\n\\s*");
  return std::regex_replace(str, newline, " ");
}

std::string MarkdownGenerator::indent(const std::string& str) {
  static const std::regex start("\\n");
  return "    " + std::regex_replace(str, start, "\n    ");
}

std::string MarkdownGenerator::stringify(const std::string& str) {
  static const std::regex quote("(\"|\\\\)");
  std::string r;
  r.append("\"");
  r.append(std::regex_replace(str, quote, "\\$1"));
  r.append("\"");
  return r;
}

std::string MarkdownGenerator::htmlize(const std::string& str) {
  static const std::regex amp("&");
  static const std::regex lt("<");
  static const std::regex gt(">");
  static const std::regex quot("\"");
  static const std::regex apos("'");
  static const std::regex ptr("\\*");

  std::string r = str;
  r = std::regex_replace(r, amp, "&amp;");  // must go first or new & replaced
  r = std::regex_replace(r, lt, "&lt;");
  r = std::regex_replace(r, gt, "&gt;");
  r = std::regex_replace(r, quot, "&quot;");
  r = std::regex_replace(r, apos, "&apos;");
  r = std::regex_replace(r, ptr, "&#42;");
  return r;
}

std::string MarkdownGenerator::sanitize(const std::string& str) {
  static const std::regex word("\\w");
  static const std::regex space("\\s");

  std::stringstream buf;
  for (auto iter = str.begin(); iter != str.end(); ++iter) {
    if (std::regex_match(iter, iter + 1, word)) {
      buf << *iter;
    } else if (std::regex_match(iter, iter + 1, space)) {
      // skip whitespace
    } else {
      /* encode non-word and non-space characters */
      buf << "_u" << std::setfill('0') << std::setw(4) << std::hex << int(*iter);
    }
  }

  /* on Linux and Mac, the maximum file name length is 255 bytes, plus leave
   * room for a four-character file extension (e.g. .html); on Windows it is
   * 260 bytes, so use the minimum */
  return buf.str().substr(0, 255 - 5);
}

std::list<const Entity*> MarkdownGenerator::view(
    const std::list<Entity>& entities, const bool sort) {
  std::list<const Entity*> ptrs;
  std::transform(entities.begin(), entities.end(), std::back_inserter(ptrs),
      [](const Entity& e) { return &e; });
  if (sort) {
    ptrs.sort([](const Entity* a, const Entity* b) {
      return a->name < b->name;
    });
  }
  return ptrs;
}
