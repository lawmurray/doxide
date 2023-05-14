#include "doxide/Generator.hpp"

void Generator::generate(const std::filesystem::path& dir, const Node& node) {
  if (node.type == NodeType::NAMESPACE) {
    generateNamespace(dir, node);
  } else if (node.type == NodeType::TYPE) {
    generateType(dir, node);
  } else {
    warn("unrecognized node type, ignoring");
  }
}

void Generator::generateNamespace(const std::filesystem::path& dir,
    const Node& node) {
  std::filesystem::create_directories(dir);
  std::ofstream out(dir / "index.md");

  /* namespace page */
  out << head("Namespace " + node.name) << std::endl;
  ++depth;
  if (node.namespaces.size() > 0) {
    out << "# Namespaces" << std::endl;
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
    line(head("Types"));
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.types) {
      out << "| [" << name << "](types/" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << "" << std::endl;
  }
  if (node.variables.size() > 0) {
    line(head("Variables"));
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.types) {
      out << "| [" << name << "](variables/" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << "" << std::endl;
  }
  if (node.operators.size() > 0) {
    line(head("Operators"));
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.operators) {
      out << "| [" << name << "](operators/" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << "" << std::endl;
  }
  if (node.functions.size() > 0) {
    line(head("Functions"));
    out << "| Name | Description |" << std::endl;
    out << "| ---- | ----------- |" << std::endl;
    for (auto& [name, child] : node.functions) {
      out << "| [" << name << "](functions/" << name << "/) | ";
      out << brief(child.docs) << " |" << std::endl;
    }
    out << std::endl;
  }
  --depth;

  /* child pages */
  for (auto& [name, child] : node.namespaces) {
    generateNamespace(dir / "namespaces" / name, child);
  }
  for (auto& [name, child] : node.types) {
    generateType(dir / "types" / name, child);
  }
  for (auto& [name, child] : node.variables) {
    generateVariable(dir / "variables" / name, child);
  }
  for (auto iter = node.operators.begin(); iter != node.operators.end();
      ++iter) {
    auto first = iter;
    iter = node.operators.upper_bound(first->first);
    generateOperator(dir / "operators" / first->first, first, iter);
  }
  for (auto iter = node.functions.begin(); iter != node.functions.end();
      ++iter) {
    auto first = iter;
    iter = node.functions.upper_bound(first->first);
    generateFunction(dir / "functions" / first->first, first, iter);
  }
}

void Generator::generateType(const std::filesystem::path& dir,
    const Node& node) {

}

void Generator::generateVariable(const std::filesystem::path& dir,
    const Node& node) {

}

template<class Iterator>
void Generator::generateFunction(const std::filesystem::path& dir,
    const Iterator& first, const Iterator& last) {
  
}

template<class Iterator>
void Generator::generateOperator(const std::filesystem::path& dir,
    const Iterator& first, const Iterator& last) {
  
}

std::string Generator::head(const std::string& name) {
  std::stringstream buf;
  buf << std::endl;
  for (int i = 0; i < depth; ++i) {
    buf << '#';
  }
  buf << ' ' << name << std::endl << std::endl;
  return buf.str();
}

std::string Generator::detailed(const std::string& str) {
  static const std::regex newline(" *\n *\\* ?");
  static const std::regex param("@t?param\\s*(\\S+)");
  static const std::regex p("@p *(\\S+)");
  static const std::regex ret("@return");
  static const std::regex see("@see");
  static const std::regex admonition("@(attention|bug|example|note|quote|todo|warning)");

  std::string r = str;
  r = std::regex_replace(r, newline, "\n");
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
  return std::regex_replace(detailed(str), std::regex("\\n"), " ");
}

std::string Generator::quote(const std::string& str,
    const std::string& indent) {
  return std::regex_replace(indent + str, std::regex("\\n"),
      std::string("\n") + indent);
}
