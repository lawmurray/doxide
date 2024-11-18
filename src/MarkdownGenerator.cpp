#include "MarkdownGenerator.hpp"
#include "YAMLParser.hpp"

MarkdownGenerator::MarkdownGenerator(const std::filesystem::path& output) :
    output(output) {
  //
}

void MarkdownGenerator::generate(const Entity& root) {
  generate(output, root);
}

void MarkdownGenerator::clean() {
  if (std::filesystem::exists(output) && std::filesystem::is_directory(output)) {
    for (auto& entry : std::filesystem::recursive_directory_iterator(output)) {
      if (entry.is_regular_file() && entry.path().extension() == ".md" &&
          !files.contains(entry)) {
        try {
          YAMLParser parser(entry.path().string());
          YAMLNode frontmatter = parser.parse();
          if (frontmatter.isValue("generator") &&
              frontmatter.value("generator") == "doxide") {
            std::filesystem::remove(entry.path());
          }
        } catch (const std::runtime_error& e) {
          warn(e.what());
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
  } else if (entity.type == EntityType::TYPE ||
      entity.type == EntityType::FILE) {
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
    try {
      YAMLParser parser(file.string());
      YAMLNode node = parser.parse();
      canWrite = node.isValue("generator") && node.value("generator") == "doxide";
    } catch (const std::runtime_error& e) {
      warn(e.what());
      canWrite = false;
    }
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
    for (auto& child : view(entity.groups, false)) {
      out << ":material-format-section: [" << title(*child) << ']';
      out << "(" << childdir << sanitize(child->name) << "/index.md)" << std::endl;
      out << ":   " << line(brief(*child)) << std::endl;
      out << std::endl;
    }

    /* namespaces */
    for (auto& child : view(entity.namespaces, true)) {
      out << ":material-package: [" << child->name << ']';
      out << "(" << childdir << sanitize(child->name) << "/index.md)" << std::endl;
      out << ":   " << line(brief(*child)) << std::endl;
      out << std::endl;
    }

    /* directories and files */
    static std::string material_file_outline("<span class=\"twemoji\"><svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\"><path d=\"M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8zm4 18H6V4h7v5h5z\"/></svg></span>");
    static std::string material_folder("<span class=\"twemoji\"><svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\"><path d=\"M10 4H4c-1.11 0-2 .89-2 2v12a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-8z\"/></svg></span>");
    auto dirs = view(entity.dirs, true);
    auto files = view(entity.files, true);
    if (dirs.size() + files.size() > 0) {
      out << "## Coverage" << std::endl;
      out << std::endl;
      out << treemap(entity) << std::endl;
      out << std::endl;

      uint32_t total_included = 0;
      uint32_t total_covered = 0;
      uint32_t total_uncovered = 0;

      out << "## Files" << std::endl;
      out << std::endl;
      out << "<table>" << std::endl;
      out << "<thead>" << std::endl;
      out << "<tr>" << std::endl;
      out << "<th style=\"text-align:left;\" data-sort-method=\"dotsep\">Name</th>" << std::endl;
      out << "<th style=\"text-align:right;\" data-sort-method=\"number\">Lines</th>" << std::endl;
      out << "<th style=\"text-align:right;\" data-sort-method=\"number\">Covered</th>" << std::endl;
      out << "<th style=\"text-align:right;\" data-sort-method=\"number\">Uncovered</th>" << std::endl;
      out << "<th style=\"text-align:right;\" data-sort-method=\"number\">Coverage</th>" << std::endl;
      out << "</tr>" << std::endl;
      out << "</thead>" << std::endl;

      out << "<tbody>" << std::endl;
      for (auto& child : dirs) {
        std::string sanitized = sanitize(child->name);
        uint32_t lines_included = child->lines_included;
        uint32_t lines_covered = child->lines_covered;
        uint32_t lines_uncovered = lines_included - lines_covered;
        double lines_percent = (lines_included > 0) ?
            100.0*lines_covered/lines_included : 0.0;
        std::string row_color = color(lines_percent);

        total_included += lines_included;
        total_covered += lines_covered;
        total_uncovered += lines_uncovered;

        /* for the purposes of table sorting, data-sort is used to prefix
         * directory names with "a." and file names with "b." to ensure that
         * directories precede files in the 'dotsep' sort order */
        out << "<tr>" << std::endl;
        out << "<td style=\"text-align:left;\" data-sort=\"a." << sanitized << "\">" << material_folder << " <a href=\"" << childdir << sanitized << "/\">" << child->name << "</a></td>" << std::endl;
        out << "<td style=\"text-align:right;\">" << lines_included << "</td>" << std::endl;
        out << "<td style=\"text-align:right;\">" << lines_covered << "</td>" << std::endl;
        out << "<td style=\"text-align:right;\">" << lines_uncovered << "</td>" << std::endl;
        out << "<td style=\"text-align:right;box-shadow: -8px 0 0 0 #" << row_color << "dd inset;\">" << std::fixed << std::setprecision(1) << lines_percent << "%</td>" << std::endl;
        out << "</tr>" << std::endl;
      }
      for (auto& child : files) {
        std::string sanitized = sanitize(child->name);
        uint32_t lines_included = child->lines_included;
        uint32_t lines_covered = child->lines_covered;
        uint32_t lines_uncovered = lines_included - lines_covered;
        double lines_percent = (lines_included > 0) ?
            100.0*lines_covered/lines_included : 0.0;
        std::string row_color = color(lines_percent);

        total_included += lines_included;
        total_covered += lines_covered;
        total_uncovered += lines_uncovered;

        /* for the purposes of table sorting, data-sort is used to prefix
         * directory names with "a." and file names with "b." to ensure that
         * directories precede files in the 'dotsep' sort order */
        out << "<tr>" << std::endl;
        out << "<td style=\"text-align:left;\" data-sort=\"b." << sanitized << "\">" << material_file_outline << " <a href=\"" << childdir << sanitized << "/\">" << child->name << "</a></td>" << std::endl;
        out << "<td style=\"text-align:right;\">" << lines_included << "</td>" << std::endl;
        out << "<td style=\"text-align:right;\">" << lines_covered << "</td>" << std::endl;
        out << "<td style=\"text-align:right;\">" << lines_uncovered << "</td>" << std::endl;
        out << "<td style=\"text-align:right;box-shadow: -8px 0 0 0 #" << row_color << "dd inset;\">" << std::fixed << std::setprecision(1) << lines_percent << "%</td>" << std::endl;
        out << "</tr>" << std::endl;
      }
      out << "</tbody>" << std::endl;

      double total_percent = (total_included > 0) ?
          100.0*total_covered/total_included : 0.0;
      std::string total_color = color(total_percent);

      out << "<tfoot>" << std::endl;
      out << "<tr>" << std::endl;
      out << "<td style=\"text-align:left;font-weight:bold;\">Total</td>" << std::endl;
      out << "<td style=\"text-align:right;font-weight:bold;\">" << total_included << "</td>" << std::endl;
      out << "<td style=\"text-align:right;font-weight:bold;\">" << total_covered << "</td>" << std::endl;
      out << "<td style=\"text-align:right;font-weight:bold;\">" << total_uncovered << "</td>" << std::endl;
      out << "<td style=\"text-align:right;font-weight:bold;box-shadow: -8px 0 0 0 #" << total_color << "dd inset;\">" << std::fixed << std::setprecision(1) << total_percent << "%</td>" << std::endl;
      out << "</tr>" << std::endl;
      out << "</tfoot>" << std::endl;

      out << "</table>" << std::endl;
      out << std::endl;
    }

    /* brief descriptions */
    auto types = view(entity.types,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP);
    if (types.size() > 0) {
      out << "## Types" << std::endl;
      out << std::endl;
      out << "| Name | Description |" << std::endl;
      out << "| ---- | ----------- |" << std::endl;
      for (auto& child : types) {
        out << "| [" << child->name << "](" << childdir << sanitize(child->name) << ".md) | ";
        out << line(brief(*child)) << " |" << std::endl;
      }
      out << std::endl;
    }

    auto concepts = view(entity.concepts,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP);
    if (concepts.size() > 0) {
      out << "## Concepts" << std::endl;
      out << std::endl;
      out << "| Name | Description |" << std::endl;
      out << "| ---- | ----------- |" << std::endl;
      for (auto& child : concepts) {
        out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
        out << line(brief(*child)) << " |" << std::endl;
      }
      out << std::endl;
    }

    auto macros = view(entity.macros,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP);
    if (macros.size() > 0) {
      out << "## Macros" << std::endl;
      out << std::endl;
      out << "| Name | Description |" << std::endl;
      out << "| ---- | ----------- |" << std::endl;
      for (auto& child : macros) {
        out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
        out << line(brief(*child)) << " |" << std::endl;
      }
      out << std::endl;
    }

    auto variables = view(entity.variables,
        entity.type == EntityType::NAMESPACE ||
        entity.type == EntityType::GROUP);
    if (variables.size() > 0) {
      out << "## Variables" << std::endl;
      out << std::endl;
      out << "| Name | Description |" << std::endl;
      out << "| ---- | ----------- |" << std::endl;
      for (auto& child : variables) {
        out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
        out << line(brief(*child)) << " |" << std::endl;
      }
      out << std::endl;
    }

    auto operators = view(entity.operators,
          entity.type == EntityType::NAMESPACE ||
          entity.type == EntityType::GROUP);
    if (operators.size() > 0) {
      out << "## Operators" << std::endl;
      out << std::endl;
      out << "| Name | Description |" << std::endl;
      out << "| ---- | ----------- |" << std::endl;
      for (auto& child : operators) {
        out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
        out << line(brief(*child)) << " |" << std::endl;
      }
      out << std::endl;
    }

    auto functions = view(entity.functions,
          entity.type == EntityType::NAMESPACE ||
          entity.type == EntityType::GROUP);
    if (functions.size() > 0) {
      out << "## Functions" << std::endl;
      out << std::endl;
      out << "| Name | Description |" << std::endl;
      out << "| ---- | ----------- |" << std::endl;
      for (auto& child : functions) {
        out << "| [" << child->name << "](#" << sanitize(child->name) << ") | ";
        out << line(brief(*child)) << " |" << std::endl;
      }
      out << std::endl;
    }

    /* for an enumerator, output the possible values */
    auto enums = view(entity.enums, false);
    if (enums.size() > 0) {
      for (auto& child : enums) {
        out << "**" << child->decl << "**" << std::endl;
        out << ":   " << child->docs << std::endl;
        out << std::endl;
      }
      out << std::endl;
    }

    /* for a file, output the complete contents */
    if (entity.type == EntityType::FILE) {
      out << "```cpp" << std::endl;
      out << entity.decl << std::endl;
      out << "```" << std::endl;
      out << std::endl;
    }

    /* detailed descriptions */
    if (concepts.size() > 0) {
      out << "## Concept Details" << std::endl;
      out << std::endl;
      for (auto& child : concepts) {
        out << "### " << child->name;
        out << "<a name=\"" << sanitize(child->name) << "\"></a>" << std::endl;
        out << std::endl;
        out << "!!! concept \"" << htmlize(line(child->decl)) << '"' << std::endl;
        out << std::endl;
        out << indent(child->docs) << std::endl;
        out << std::endl;
      }
    }
    if (macros.size() > 0) {
      out << "## Macro Details" << std::endl;
      out << std::endl;
      for (auto& child : macros) {
        out << "### " << child->name;
        out << "<a name=\"" << sanitize(child->name) << "\"></a>" << std::endl;
        out << std::endl;
        out << "!!! macro \"" << htmlize(line(child->decl)) << '"' << std::endl;
        out << std::endl;
        out << indent(child->docs) << std::endl;
        out << std::endl;
      }
    }
    if (variables.size() > 0) {
      out << "## Variable Details" << std::endl;
      out << std::endl;
      for (auto& child : variables) {
        out << "### " << child->name;
        out << "<a name=\"" << sanitize(child->name) << "\"></a>" << std::endl;
        out << std::endl;
        out << "!!! variable \"" << htmlize(line(child->decl)) << '"' << std::endl;
        out << std::endl;
        out << indent(child->docs) << std::endl;
        out << std::endl;
      }
    }
    if (operators.size() > 0) {
      out << "## Operator Details" << std::endl;
      out << std::endl;
      std::string prev;
      for (auto& child : operators) {
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
    if (functions.size() > 0) {
      out << "## Function Details" << std::endl;
      out << std::endl;
      std::string prev;
      for (auto& child : functions) {
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
  for (auto& child : view(entity.groups, false)) {
    generate(output / name, *child);
  }
  for (auto& child : view(entity.namespaces, false)) {
    generate(output / name, *child);
  }
  for (auto& child : view(entity.types, false)) {
    generate(output / name, *child);
  }
  for (auto& child : view(entity.dirs, false)) {
    generate(output / name, *child);
  }
  for (auto& child : view(entity.files, false)) {
    generate(output / name, *child);
  }
}

std::string MarkdownGenerator::treemap(const Entity& entity) {
  std::stringstream buf;
  buf <<
  R""""(
  <div id="coverage-sunburst" style="width:600px;height:600px;max-width:100%;max-height:auto;"></div>
  <script src="https://cdn.jsdelivr.net/npm/echarts@5.5.1/dist/echarts.min.js"></script>
  <script type="text/javascript">
  )"""";
  buf << "var data = [" << treemap_data(entity) << ']' << std::endl;
  buf <<
  R""""(
  var coverage_sunburst = echarts.init(document.getElementById('coverage-sunburst'));
  var option = {
    series: {
      type: 'sunburst',
      data: data,
      sort: null,
      radius: ['20%', '95%'],
      startAngle: 0,
      clockwise: false,
      itemStyle: {
        borderWidth: 1
      },
      label: {
        color: 'white',
        fontSize: 10,
        textBorderWidth: 1,
        align: 'left',
        rotate: 'radial',
        width: 50,
        overflow: 'truncate'
      },
      labelLayout: {
        hideOverlap: true
      },
      levels: [
        {
          itemStyle: {
            opacity: 0.1
          }
        }
      ]
    }
  };
  coverage_sunburst.setOption(option);
  window.addEventListener("resize", () => {
    coverage_sunburst.resize();
  });
  </script>
  )"""";

  return buf.str();
}

std::string MarkdownGenerator::treemap_data(const Entity& entity) {
  std::stringstream buf;
  bool first = true;
  for (auto& dir: view(entity.dirs, true)) {
    double percent = (dir->lines_included > 0) ?
        100.0*dir->lines_covered/dir->lines_included : 0.0;
    std::string c = color(percent);

    if (!first) {
      buf << ", ";
    }
    first = false;

    buf << '{';
    buf << "name: \"" << dir->name << "\", ";
    buf << "path: \"" << dir->filename << "\", ";
    buf << "value: " << dir->lines_included << ", ";
    buf << "children: [" << treemap_data(*dir) << "], ";
    buf << "itemStyle: { color: \"#" << c << "dd\", borderColor: \"#" << c << "\"}, ";
    buf << "label: { textBorderColor: \"#" << c << "\"}";
    buf << '}';
  }
  for (auto& file: view(entity.files, true)) {
    double percent = (file->lines_included > 0) ?
        100.0*file->lines_covered/file->lines_included : 0.0;
    std::string c = color(percent);

    if (!first) {
      buf << ", ";
    }
    first = false;

    buf << '{';
    buf << "name: \"" << file->name << "\", ";
    buf << "path: \"" << file->filename << "\", ";
    buf << "value: " << file->lines_included << ", ";
    buf << "itemStyle: { color: \"#" << c << "dd\", borderColor: \"#" << c << "\"}, ";
    buf << "label: { textBorderColor: \"#" << c << "\"}";
    buf << '}';
  }
  return buf.str();
}

std::string MarkdownGenerator::frontmatter(const Entity& entity) {
  /* use YAML frontmatter to ensure correct capitalization of title, and to
   * mark as managed by Doxide */
  std::stringstream buf;
  buf << "---" << std::endl;
  buf << "title: " << stringify(title(entity)) << std::endl;
  buf << "description: " << stringify(line(brief(entity))) << std::endl;
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
    static const std::regex reg("^(`.*?`|[^;:.?!])*[\\.\\?\\!](?=\\s|$)");
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
  /* basic replacements */
  static const std::regex amp("&");
  static const std::regex lt("<");
  static const std::regex gt(">");
  static const std::regex quot("\"");
  static const std::regex apos("'");
  static const std::regex ptr("\\*");

  /* the sequence operator[](...) looks like a link in Markdown */
  static const std::regex operator_brackets("operator\\[\\]");

  std::string r = str;
  r = std::regex_replace(r, amp, "&amp;");  // must go first or new & replaced
  r = std::regex_replace(r, lt, "&lt;");
  r = std::regex_replace(r, gt, "&gt;");
  r = std::regex_replace(r, quot, "&quot;");
  r = std::regex_replace(r, apos, "&apos;");
  r = std::regex_replace(r, ptr, "&#42;");
  r = std::regex_replace(r, operator_brackets, "operator&#91;&#93;");
  return r;
}

std::string MarkdownGenerator::sanitize(const std::string& str) {
  static const std::regex word("\\w|[.]");
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

std::string MarkdownGenerator::color(const double percent) {
  assert(0.0 <= percent && percent <= 100.0);

  const int base_red = 0xEF5552;
  const int base_yellow = 0xFFC105;
  const int base_green = 0x4CAE4F;

  double rounded_percent = 10.0*std::floor(percent/10.0);
  int from, to;
  double mix;
  if (rounded_percent < 50.0) {
    from = base_red;
    to = base_red;
    mix = 1.0;
  } else if (rounded_percent < 70.0) {
    from = base_red;
    to = base_yellow;
    mix = (rounded_percent - 50.0)/20.0;
  } else if (rounded_percent < 90.0) {
    from = base_yellow;
    to = base_green;
    mix = (rounded_percent - 70.0)/20.0;
  } else {
    from = base_green;
    to = base_green;
    mix = 1.0;
  }

  int r = (1.0 - mix)*((from >> 16) & 0xFF) + mix*((to >> 16) & 0xFF);
  int g = (1.0 - mix)*((from >> 8) & 0xFF) + mix*((to >> 8) & 0xFF);
  int b = (1.0 - mix)*((from >> 0) & 0xFF) + mix*((to >> 0) & 0xFF);
  int c = (r << 16) | (g << 8) | b;

  std::stringstream result;
  result << std::hex << std::setw(6) << std::setfill('0') << c;
  return result.str();
}

std::list<const Entity*> MarkdownGenerator::view(
    const std::list<Entity>& entities, const bool sort) {
  std::list<const Entity*> ptrs;
  std::transform(entities.begin(), entities.end(), std::back_inserter(ptrs),
      [](const Entity& e) { return &e; });
  auto end = std::remove_if(ptrs.begin(), ptrs.end(),
      [](const Entity* e) { 
        return e->hide ||
          (e->type != EntityType::DIR && e->type != EntityType::FILE && e->docs.empty()) || 
          (e->type == EntityType::NAMESPACE && !e->visibleChildren); });
  ptrs.erase(end, ptrs.end());
  if (sort) {
    ptrs.sort([](const Entity* a, const Entity* b) {
      return a->name < b->name;
    });
  }
  return ptrs;
}
