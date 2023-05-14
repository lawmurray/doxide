#include "doxide/Generator.hpp"

void Generator::generate(const std::filesystem::path& dir,
    const Node& global) {
  generateNamespace(dir, global);
}

void Generator::generateNamespace(const std::filesystem::path& dir,
    const Node& node) {
  std::filesystem::create_directories(dir);
  std::ofstream out;
  if (node.name.empty()) {
    out.open(dir / "index.md", std::ios::app);
  } else {
    out.open(dir / (node.name + ".md"));
    out << "# Namespace " << node.name << std::endl;
  }

  /* namespace page */
  out << std::endl;
  if (node.namespaces.size() > 0) {
    out << "## Namespaces" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.namespaces) {
      out << "| [" << name << "](" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << "" << std::endl;
  }
  if (node.types.size() > 0) {
    out << "## Types" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.types) {
      out << "| [" << name << "](types/" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << "" << std::endl;
  }
  if (node.variables.size() > 0) {
    out << "## Variables" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.variables) {
      out << "| [" << name << "](variables/" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << "" << std::endl;
  }
  if (node.operators.size() > 0) {
    out << "## Operators" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.operators) {
      out << "| [" << name << "](operators/" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << "" << std::endl;
  }
  if (node.functions.size() > 0) {
    out << "## Functions" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.functions) {
      out << "| [" << name << "](functions/" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }

  /* child pages */
  for (auto& [name, child] : node.namespaces) {
    generateNamespace(dir, child);
  }
  for (auto& [name, child] : node.types) {
    generateType(dir / "types", child);
  }
  for (auto& [name, child] : node.variables) {
    generateVariable(dir / "variables", child);
  }
  for (auto iter = node.operators.begin(); iter != node.operators.end(); ) {
    auto first = iter;
    do {
      ++iter;
    } while (iter != node.operators.end() && iter->first == first->first);
    generateOperator(dir / "operators", first, iter);
  }
  for (auto iter = node.functions.begin(); iter != node.functions.end(); ) {
    auto first = iter;
    do {
      ++iter;
    } while (iter != node.functions.end() && iter->first == first->first);
    generateFunction(dir / "functions", first, iter);
  }
}

void Generator::generateType(const std::filesystem::path& dir,
    const Node& node) {
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (node.name + ".md"));

  /* type page */
  out << "# " + node.name << std::endl;
  out << std::endl;
  out << "**" << line(node.decl) << "**" << std::endl;
  out << std::endl;
  out << detailed(node.docs) << std::endl;
  out << std::endl;

  /* brief descriptions */
  if (node.variables.size() > 0) {
    out << "## Member Variables" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.variables) {
      out << "| [" << name << "](#" << name << ") | ";
      out << line(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (node.operators.size() > 0) {
    out << "## Member Operators" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.operators) {
      out << "| [" << name << "](#" << name << ") | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  if (node.functions.size() > 0) {
    out << "## Member Functions" << std::endl;
    out << std::endl;
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.functions) {
      out << "| [" << name << "](#" << name << ") | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }

  /* detailed descriptions */
  if (node.variables.size() > 0) {
    out << "## Member Variable Details" << std::endl;
    out << std::endl;
    for (auto& [name, child] : node.variables) {
      out << "### " << name << std::endl;
      out << std::endl;
      out << "!!! abstract \"" << line(child.decl) << '"' << std::endl;
      out << std::endl;
      out << indent(detailed(child.docs)) << std::endl;
      out << std::endl;
    }
  }
  if (node.operators.size() > 0) {
    out << "## Member Operator Details" << std::endl;
    out << std::endl;
    std::string prev;
    for (auto& [name, child] : node.operators) {
      if (name != prev) {
        /* heading only for the first overload of this name */
        out << "### " << name << std::endl;
        out << std::endl;
      }
      out << "!!! abstract \"" << line(child.decl) << '"' << std::endl;
      out << std::endl;
      out << indent(detailed(child.docs)) << std::endl;
      out << std::endl;
    }
  }
  if (node.functions.size() > 0) {
    out << "## Member Function Details" << std::endl;
    out << std::endl;
    std::string prev;
    for (auto& [name, child] : node.functions) {
      if (name != prev) {
        /* heading only for the first overload of this name */
        out << "### " << name << std::endl;
        out << std::endl;
      }
      out << "!!! abstract \"" << line(child.decl) << '"' << std::endl;
      out << std::endl;
      out << indent(detailed(child.docs)) << std::endl;
      out << std::endl;
    }
  }
}

void Generator::generateVariable(const std::filesystem::path& dir,
    const Node& node) {
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (node.name + ".md"));

  out << "# " << node.name << std::endl;
  out << std::endl;
  out << "!!! abstract \"" << line(node.decl) << '"' << std::endl;
  out << std::endl;
  out << indent(detailed(node.docs)) << std::endl;
  out << std::endl;
}

template<class Iterator>
void Generator::generateFunction(const std::filesystem::path& dir,
    const Iterator& first, const Iterator& last) {
  auto& node = first->second;
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (node.name + ".md"));

  out << "# " << node.name << std::endl;
  out << std::endl;
  for (auto iter = first; iter != last; ++iter) {
    auto& node = iter->second;
    out << "!!! abstract \"" << line(node.decl) << '"' << std::endl;
    out << std::endl;
    out << indent(detailed(node.docs)) << std::endl;
    out << std::endl;
  }
}

template<class Iterator>
void Generator::generateOperator(const std::filesystem::path& dir,
    const Iterator& first, const Iterator& last) {
  auto& node = first->second;
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / (node.name + ".md"));

  out << "# " << node.name << std::endl;
  out << std::endl;
  for (auto iter = first; iter != last; ++iter) {
    auto& node = iter->second;
    out << "!!! abstract \"" << line(node.decl) << '"' << std::endl;
    out << std::endl;
    out << indent(detailed(node.docs)) << std::endl;
    out << std::endl;
  }
}

std::string Generator::detailed(const std::string& str) {
  static const std::regex start("^/\\*\\*\\s*");
  static const std::regex line("(^|\\n)[ \t]*\\*[ \t]*");
  static const std::regex end("\\s*/$");
  static const std::regex param("@t?param[ \t]+(\\S+)");
  static const std::regex p("@p[ \t]+(\\S+)");
  static const std::regex ret("@return");
  static const std::regex see("@see");
  static const std::regex admonition("@(attention|bug|example|note|quote|todo|warning)");

  std::string r = str;
  r = std::regex_replace(r, start, "");
  r = std::regex_replace(r, line, "$1");
  r = std::regex_replace(r, end, "");
  r = std::regex_replace(r, param, "  - **$1** ");
  r = std::regex_replace(r, p, "**$1**");
  r = std::regex_replace(r, ret, "**Returns** ");
  r = std::regex_replace(r, see, "**See also** ");
  r = std::regex_replace(r, admonition, "!!! $1");
  return r;
}

std::string Generator::brief(const std::string& str) {
  static const std::regex reg("^.*?[\\.\\?\\!]");

  std::string l = line(str);
  std::smatch match;
  if (std::regex_search(l, match, reg)) {
    return match.str();
  } else {
    return "";
  }
}

std::string Generator::line(const std::string& str) {
  static const std::regex newline("\\s*\\n\\s*");
  return std::regex_replace(detailed(str), newline, " ");
}

std::string Generator::indent(const std::string& str,
    const std::string& indent) {
  static const std::regex start("\\n");
  return "    " + std::regex_replace(str, start, "\n    ");
}
