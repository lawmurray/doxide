#include "MarkdownGenerator.hpp"
#include "YAMLParser.hpp"

void MarkdownGenerator::generate(const std::filesystem::path& output,
    const Entity& entity) {
  std::string name = sanitize(entity.name);  // entity name, empty for root
  std::string dirname;  // directory name for this entity
  std::string filename;  // file name for this entity
  std::string childdir;  // directory name for children, relative to filename
  if (name.empty()) {
    /* root node */
    dirname = "";
    filename = "index";
    childdir = "";
  } else if (entity.type == EntityType::TYPE) {
    /* when building the navigation menu, mkdocs modifies directory names by
     * replacing underscores and capitalizing words; this is problematic for
     * type names; rather than generating the docs for a class at
     * ClassName/index.md, they are generated at ClassName.md, but children
     * still go in a ClassName/ subdirectory; final URLs are the same, but
     * this unfortunately adds two links to the menu, one for the class,
     * another with the same name that expands for the children, if they
     * exist; this is not such a problem for classes, which don't often have
     * children, so we prefer the option... */
    dirname = "";
    filename = name;
    childdir = name + "/";
  } else {
    /* ...whereas namespaces and groups tend to have simpler names not
     * affected by the mkdocs changes, while two links in the menu is a
     * more unsightly, so don't change them */
    dirname = name;
    filename = "index";
    childdir = "";
  }
  std::ofstream out;
  std::filesystem::create_directories(output / dirname);
  std::filesystem::path file = output / dirname / (filename + ".md");

  /* check if the file exists, and if so that it can be overwritten */
  bool canWrite = true;
  if (std::filesystem::exists(file)) {
    YAMLParser parser;
    YAMLNode frontmatter = parser.parse(file.string());
    canWrite = frontmatter.isValue("generator") && frontmatter.value("generator") == "doxide";
  }
  if (!canWrite) {
    warn(file.string() << " already exists and was not generated by doxide, will not overwrite");
  } else {
    files.insert(file);
    out.open(file);
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
      out << "(" << childdir << sanitize(child.name) << "/index.md)" << std::endl;
      out << ":   " << line(brief(child)) << std::endl;
      out << std::endl;
    }

    /* namespaces */
    for (auto& child : view(entity.namespaces, true)) {
      if (!child->empty()) {
        out << ":material-package: [" << child->name << ']';
        out << "(" << childdir << sanitize(child->name) << "/index.md)" << std::endl;
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
        out << "| [" << child->name << "](" << childdir << sanitize(child->name) << ".md) | ";
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
  }

  /* child pages */
  std::filesystem::create_directories(output / name);
  for (auto& child : entity.groups) {
    generate(output / name, child);
  }
  for (auto& child : entity.namespaces) {
    if (!child.empty()) {
      generate(output / name, child);
    }
  }
  for (auto& child : entity.types) {
    generate(output / name, child);
  }
}

void MarkdownGenerator::clean(const std::filesystem::path& output) {
  if (std::filesystem::exists(output) && std::filesystem::is_directory(output)) {
    for (auto& entry : std::filesystem::recursive_directory_iterator(output)) {
      if (entry.is_regular_file() && entry.path().extension() == ".md" &&
          !files.contains(entry)) {
        try {
          YAMLParser parser;
          YAMLNode frontmatter = parser.parse(entry.path().string());
          if (frontmatter.isValue("generator") &&
              frontmatter.value("generator") == "doxide") {
            std::filesystem::remove(entry.path());
          }
        } catch (const std::runtime_error&) {
          // ignore
        }
      }
    }

    /* traverse the output directory again, this time removing any empty
     * directories; because removing a directory may make its parent directory
     * empty, repeat until there are no further empty directories */
    std::vector<std::filesystem::path> empty;
    do {
      empty.clear();
      for (auto& entry : std::filesystem::recursive_directory_iterator(output)) {
        if (entry.is_directory() && std::filesystem::is_empty(entry.path())) {
          empty.push_back(entry.path());
        }
      }
      for (auto& dir : empty) {
        std::filesystem::remove(dir);
      }    
    } while (empty.size());
  }
}

std::string MarkdownGenerator::frontmatter(const Entity& entity) {
  /* use YAML frontmatter to ensure correct capitalization of title, and to
   * mark as managed by Doxide */
  std::stringstream buf;
  buf << "---" << std::endl;
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
    static const std::regex reg("^(`.*?`|[^;:.?!])*[\\.\\?\\!]");
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
