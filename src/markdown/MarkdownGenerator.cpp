#include "markdown/MarkdownGenerator.hpp"

void MarkdownGenerator::generate(const std::filesystem::path& dir,
    const Entity& global) {
  generateGroup(dir, global);
}

void MarkdownGenerator::generateGroup(const std::filesystem::path& dir,
    const Entity& entity) {
  std::ofstream out;
  if (entity.name.empty()) {
    std::filesystem::create_directories(dir);
    out.open(dir / "index.md", std::ios::app);
  } else {
    std::filesystem::create_directories(dir / sanitize(entity.name));
    out.open(dir / sanitize(entity.name) / "index.md");
    out << frontmatter(entity) << std::endl;

    /* header */
    out << "# " << entity.name << std::endl;
    out << std::endl;
  }

  /* groups */
  if (entity.groups.size() > 0) {
    out << "## Groups" << std::endl;
    for (auto& [name, child] : entity.groups) {
      out << ":material-view-module-outline: [" << name << "](" << sanitize(name) << "/)" << std::endl;
      out << ":   " << child.docs << std::endl;
      out << std::endl;
    }
  }

  if (entity.type == EntityType::NAMESPACE) {
    /* namespace page */
    out << "**" << htmlize(line(entity.decl)) << "**" << std::endl;
    out << std::endl;
    out << entity.docs << std::endl;
    out << std::endl;
  }

  if (entity.namespaces.size() > 0) {
    out << "## Namespaces" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.namespaces) {
      out << "| [" << name << "](" << sanitize(name) << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.macros.size() > 0) {
    out << "## Macros" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.macros) {
      out << "| [" << name << "](" << sanitize(name) << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.types.size() > 0) {
    out << "## Types" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.types) {
      out << "| [" << name << "](types/" << sanitize(name) << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.variables.size() > 0) {
    out << "## Variables" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.variables) {
      out << "| [" << name << "](variables/" << sanitize(name) << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.operators.size() > 0) {
    out << "## Operators" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.operators) {
      out << "| [" << name << "](operators/" << sanitize(name) << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.functions.size() > 0) {
    out << "## Functions" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.functions) {
      out << "| [" << name << "](functions/" << sanitize(name) << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }

  /* child pages */
  for (auto& [name, child] : entity.groups) {
    generateGroup(dir / sanitize(entity.name), child);
  }
  for (auto& [name, child] : entity.namespaces) {
    generateGroup(dir / sanitize(entity.name), child);
  }
  for (auto& [name, child] : entity.macros) {
    generateMacro(dir / sanitize(entity.name), child);
  }
  for (auto& [name, child] : entity.types) {
    generateType(dir / sanitize(entity.name) / "types", child);
  }
  for (auto& [name, child] : entity.variables) {
    generateVariable(dir / sanitize(entity.name) / "variables", child);
  }
  for (auto iter = entity.operators.begin(); iter != entity.operators.end(); ) {
    auto first = iter;
    do {
      ++iter;
    } while (iter != entity.operators.end() && iter->first == first->first);
    generateOperator(dir / sanitize(entity.name) / "operators", first, iter);
  }
  for (auto iter = entity.functions.begin(); iter != entity.functions.end(); ) {
    auto first = iter;
    do {
      ++iter;
    } while (iter != entity.functions.end() && iter->first == first->first);
    generateFunction(dir / sanitize(entity.name) / "functions", first, iter);
  }
}

void MarkdownGenerator::generateMacro(const std::filesystem::path& dir,
    const Entity& entity) {
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (sanitize(entity.name) + ".md"));
  out << frontmatter(entity) << std::endl;
  out << "# " << entity.name << std::endl;
  out << std::endl;
  out << "!!! macro \"" << htmlize(line(entity.decl)) << '"' << std::endl;
  out << std::endl;
  out << indent(entity.docs) << std::endl;
  out << std::endl;
}

void MarkdownGenerator::generateType(const std::filesystem::path& dir,
    const Entity& entity) {
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (sanitize(entity.name) + ".md"));
  out << frontmatter(entity) << std::endl;

  /* type page */
  out << "# " + entity.name << std::endl;
  out << std::endl;
  out << "**" << htmlize(line(entity.decl)) << "**" << std::endl;
  out << std::endl;
  out << entity.docs << std::endl;
  out << std::endl;

  /* for an enumerator, output the possible values */
  if (entity.enumerators.size() > 0) {
    for (auto& [name, child] : entity.enumerators) {
      out << "**" << name << "**" << std::endl;
      out << ":   " << child.docs << std::endl;
      out << std::endl;
    }
    out << std::endl;
  }

  /* brief descriptions */
  if (entity.variables.size() > 0) {
    out << "## Member Variables" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.variables) {
      out << "| [" << name << "](#" << sanitize(name) << ") | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.operators.size() > 0) {
    out << "## Member Operators" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.operators) {
      out << "| [" << name << "](#" << sanitize(name) << ") | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (entity.functions.size() > 0) {
    out << "## Member Functions" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : entity.functions) {
      out << "| [" << name << "](#" << sanitize(name) << ") | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }

  /* detailed descriptions */
  if (entity.variables.size() > 0) {
    out << "## Member Variable Details" << std::endl;
    out << std::endl;
    for (auto& [name, child] : entity.variables) {
      //out << "### " << name;
      out << "<a name=\"" << sanitize(name) << "\"></a>" << std::endl;
      out << std::endl;
      out << "!!! variable \"" << htmlize(line(child.decl)) << '"' << std::endl;
      out << std::endl;
      out << indent(child.docs) << std::endl;
      out << std::endl;
    }
  }
  if (entity.operators.size() > 0) {
    out << "## Member Operator Details" << std::endl;
    out << std::endl;
    std::string prev;
    for (auto& [name, child] : entity.operators) {
      if (name != prev) {
        /* heading only for the first overload of this name */
        //out << "### " << name;
        out << "<a name=\"" << sanitize(name) << "\"></a>" << std::endl;
        out << std::endl;
      }
      out << "!!! function \"" << htmlize(line(child.decl)) << '"' << std::endl;
      out << std::endl;
      out << indent(child.docs) << std::endl;
      out << std::endl;
      prev = name;
    }
  }
  if (entity.functions.size() > 0) {
    out << "## Member Function Details" << std::endl;
    out << std::endl;
    std::string prev;
    for (auto& [name, child] : entity.functions) {
      if (name != prev) {
        /* heading only for the first overload of this name */
        //out << "### " << name;
        out << "<a name=\"" << sanitize(name) << "\"></a>" << std::endl;
      }
      out << "!!! function \"" << htmlize(line(child.decl)) << '"' << std::endl;
      out << std::endl;
      out << indent(child.docs) << std::endl;
      out << std::endl;
      prev = name;
    }
  }
}

void MarkdownGenerator::generateVariable(const std::filesystem::path& dir,
    const Entity& entity) {
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (sanitize(entity.name) + ".md"));
  out << frontmatter(entity) << std::endl;
  out << "# " << entity.name << std::endl;
  out << std::endl;
  out << "!!! variable \"" << htmlize(line(entity.decl)) << '"' << std::endl;
  out << std::endl;
  out << indent(entity.docs) << std::endl;
  out << std::endl;
}

template<class Iterator>
void MarkdownGenerator::generateFunction(const std::filesystem::path& dir,
    const Iterator& first, const Iterator& last) {
  auto& entity = first->second;
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (sanitize(entity.name) + ".md"));
  out << frontmatter(entity) << std::endl;
  out << "# " << entity.name << std::endl;
  out << std::endl;
  for (auto iter = first; iter != last; ++iter) {
    auto& entity = iter->second;
    out << "!!! function \"" << htmlize(line(entity.decl)) << '"' << std::endl;
    out << std::endl;
    out << indent(entity.docs) << std::endl;
    out << std::endl;
  }
}

template<class Iterator>
void MarkdownGenerator::generateOperator(const std::filesystem::path& dir,
    const Iterator& first, const Iterator& last) {
  auto& entity = first->second;
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (sanitize(entity.name) + ".md"));
  out << frontmatter(entity) << std::endl;
  out << "# " << entity.name << std::endl;
  out << std::endl;
  for (auto iter = first; iter != last; ++iter) {
    auto& entity = iter->second;
    out << "!!! function \"" << htmlize(line(entity.decl)) << '"' << std::endl;
    out << std::endl;
    out << indent(entity.docs) << std::endl;
    out << std::endl;
  }
}

std::string MarkdownGenerator::frontmatter(const Entity& entity) {
  /* use YAML frontmatter to ensure correct capitalization of title, and to
   * mark as managed by Doxide */
  std::stringstream buf;
  buf << "title: " << entity.name << std::endl;
  buf << "generator: doxide" << std::endl;
  buf << "---" << std::endl;
  buf << std::endl;
  return buf.str();
}

std::string MarkdownGenerator::brief(const std::string& str) {
  static const std::regex reg("^.*?[\\.\\?\\!]");

  std::string l = line(str);
  std::smatch match;
  if (std::regex_search(l, match, reg)) {
    return match.str();
  } else {
    return "";
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

std::string MarkdownGenerator::htmlize(const std::string& str) {
  static const std::regex amp("&");
  static const std::regex lt("<");
  static const std::regex gt(">");
  static const std::regex quot("\"");
  static const std::regex apos("'");

  std::string r = str;
  r = std::regex_replace(r, amp, "&amp;");  // must go first or new & replaced
  r = std::regex_replace(r, lt, "&lt;");
  r = std::regex_replace(r, gt, "&gt;");
  r = std::regex_replace(r, quot, "&quot;");
  r = std::regex_replace(r, apos, "&apos;");
  return r;
}

std::string MarkdownGenerator::sanitize(const std::string& str) {
  static const std::regex word("\\w");

  std::stringstream buf;
  for (auto iter = str.begin(); iter != str.end(); ++iter) {
    if (std::regex_match(iter, iter + 1, word)) {
      buf << *iter;
    } else {
      buf << "_u" << std::setfill('0') << std::setw(4) << std::hex << int(*iter);
    }
  }
  return buf.str();
}
