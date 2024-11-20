#include "MarkdownGenerator.hpp"
#include "YAMLParser.hpp"

MarkdownGenerator::MarkdownGenerator(const std::filesystem::path& output) :
    output(output) {
  //
}

void MarkdownGenerator::generate(const Entity& root) {
  generate(output, root);
}

void MarkdownGenerator::coverage(const Entity& root) {
  coverage(output, root);
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
  if (entity.type == EntityType::NONE) {
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

  std::filesystem::create_directories(output / dirname);
  std::filesystem::path file = output / dirname / (filename + ".md");
  if (can_write(file)) {
    files.insert(file);
    std::ofstream out(file);

    /* frontmatter*/
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

    /* code coverage */
    if (entity.type == EntityType::NONE) {
      out << ":material-chart-pie: [Code Coverage](coverage/index.md)" << std::endl;
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
}

void MarkdownGenerator::coverage(const std::filesystem::path& output,
    const Entity& entity) {
  std::string name = sanitize(entity.type == EntityType::NONE ?
      "coverage" : entity.name);  // entity name, "coverage"" for root
  std::string dirname;  // directory name for this entity
  std::string filename;  // file name for this entity
  std::string childdir;  // directory name for children, relative to filename
  if (entity.type == EntityType::FILE) {
    /* as for types in generate(), working around some mkdocs behavior */
    dirname = "";
    filename = name;
    childdir = name + "/";
  } else {
    dirname = name;
    filename = "index";
    childdir = "";
  }

  std::filesystem::create_directories(output / dirname);
  std::filesystem::path file = output / dirname / (filename + ".md");
  if (can_write(file)) {
    files.insert(file);
    std::ofstream out(file);

    /* frontmatter*/
    out << frontmatter(entity) << std::endl;

    /* header */
    if (entity.type == EntityType::NONE) {
      out << "# Code Coverage" << std::endl;
      out << std::endl;
    } else {
      out << "# " << title(entity) << std::endl;
      out << std::endl;
      out << entity.docs << std::endl;
      out << std::endl;
    }

    if (entity.type == EntityType::NONE || entity.type == EntityType::DIR) {
      /* code coverage chart */
      sunburst(entity, entity, out);
      out << std::endl;

      /* code coverage table */
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
      coverage_data(entity, entity, out);
      out << "</tbody>" << std::endl;
      out << "<tfoot>" << std::endl;
      coverage_foot(entity, entity, out);
      out << "</tfoot>" << std::endl;
      out << "</table>" << std::endl;
      out << std::endl;
    } else if (entity.type == EntityType::FILE) {
      /* for a file, output the whole contents; line numbers are added with
       * the Markdown notation `linenums="1"`, which ultimately creates a HTML
       * <table> of one row <tr>, with two cells <td>, the first holding the
       * line numbers and the second the code; the line number cell contains
       * a further <pre> with one empty <span></span>, then each line number
       * in a separate <span> within it; to indicate code coverage we apply
       * styles to those <span>, which are readily selected by number using
       * the CSS nth-child() pseudo-class */
      std::string covered_color = color(100.0);
      std::string uncovered_color = color(0.0);

      /* style sheet to highlight lines according to code coverage */
      out << "<style>" << std::endl;
      out << ".linenodiv pre span {";
      out << "padding-left:4px;padding-right:4px";
      out << "}" << std::endl;
      for (uint32_t line = 0; line < entity.line_counts.size(); ++line) {
        if (entity.line_counts[line] > 0) {
          /* line is covered; line + 2 here to account for 0-base to 1-base
          * conversion plus the extra empty <span></span> mentioned above */
          out << ".linenodiv pre span:nth-child(" << (line + 2) << ") {";
          out << "background-color: #" << covered_color << "dd;";
          out << "color: white;";
          out << "}" << std::endl;
          out << ".linenodiv pre span:nth-child(" << (line + 2) << ")::before {";
          out << "content: \"● \";";
          out << "}" << std::endl;
        } else if (entity.line_counts[line] == 0) {
          /* line is uncovered; line + 2 here to account for 0-base to 1-base
          * conversion plus the extra empty <span></span> mentioned above */
          out << ".linenodiv pre span:nth-child(" << (line + 2) << ") {";
          out << "background-color: #" << uncovered_color << "dd;";
          out << "color: white;";
          out << "}" << std::endl;
          out << ".linenodiv pre span:nth-child(" << (line + 2) << ")::before {";
          out << "content: \"○ \";";
          out << "}" << std::endl;
        }
      }
      out << "</style>" << std::endl;
      out << std::endl;

      /* lines numbers and source code */
      out << "```cpp linenums=\"1\"" << std::endl;
      out << entity.decl << std::endl;
      out << "```" << std::endl;
      out << std::endl;
    }
  }

  /* child pages */
  std::filesystem::create_directories(output / name);
  for (auto& child : view(entity.dirs, false)) {
    coverage(output / name, *child);
  }
  for (auto& child : view(entity.files, false)) {
    coverage(output / name, *child);
  }
}

void MarkdownGenerator::coverage_data(const Entity& entity,
    const Entity& root, std::ofstream& out) {
  /* icons */
  static std::string material_file_outline("<span class=\"twemoji\"><svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\"><path d=\"M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8zm4 18H6V4h7v5h5z\"/></svg></span>");
  static std::string material_folder("<span class=\"twemoji\"><svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\"><path d=\"M10 4H4c-1.11 0-2 .89-2 2v12a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-8z\"/></svg></span>");

  /* <tr> includes a data-parent attribute to facilitate filtering the table
   * dynamically based on the currently-selected directory */

  /* <td> includes data-sort (used by tablesort) for the names column to
   * prefix directory names with "a." and file names with "b." for the
   * purposes of sorting using the 'dotsep' sort order */

  auto dirs = view(entity.dirs, true);
  for (auto& child : dirs) {
    std::string parent = sanitize(relative(child->path.parent_path(), root.path));
    std::string name = sanitize(child->name);
    uint32_t lines_included = child->lines_included;
    uint32_t lines_covered = child->lines_covered;
    uint32_t lines_uncovered = lines_included - lines_covered;
    double lines_percent = (lines_included > 0) ?
        100.0*lines_covered/lines_included : 0.0;
    std::string lines_color = color(lines_percent);
    std::string path = parent.empty() ? name : parent + '/' + name;
    std::string style = parent.empty() ? "" : " style=\"display:none;\"";

    out << "<tr id=\"" << path << "\" data-parent=\"" << parent << "\"" << style << ">" << std::endl;
    out << "<td style=\"text-align:left;\" data-sort=\"a." << name << "\">" << material_folder << " <a href=\"" << path << "/\">" << htmlize(child->name) << "</a></td>" << std::endl;
    out << "<td style=\"text-align:right;\">" << lines_included << "</td>" << std::endl;
    out << "<td style=\"text-align:right;\">" << lines_covered << "</td>" << std::endl;
    out << "<td style=\"text-align:right;\">" << lines_uncovered << "</td>" << std::endl;
    out << "<td style=\"text-align:right;box-shadow: -8px 0 0 0 #" << lines_color << "dd inset;\">" << std::fixed << std::setprecision(1) << lines_percent << "%</td>" << std::endl;
    out << "</tr>" << std::endl;

    coverage_data(*child, root, out);
  }

  auto files = view(entity.files, true);
  for (auto& child : files) {
    std::string parent = sanitize(relative(child->path.parent_path(), root.path));
    std::string name = sanitize(child->name);
    uint32_t lines_included = child->lines_included;
    uint32_t lines_covered = child->lines_covered;
    uint32_t lines_uncovered = lines_included - lines_covered;
    double lines_percent = (lines_included > 0) ?
        100.0*lines_covered/lines_included : 0.0;
    std::string lines_color = color(lines_percent);
    std::string path = parent.empty() ? name : parent + '/' + name;
    std::string style = parent.empty() ? "" : " style=\"display:none;\"";

    out << "<tr id=\"" << path << "\" data-parent=\"" << parent << "\"" << style << ">" << std::endl;
    out << "<td style=\"text-align:left;\" data-sort=\"b." << name << "\">" << material_file_outline << " <a href=\"" << path << "/\">" << htmlize(child->name) << "</a></td>" << std::endl;
    out << "<td style=\"text-align:right;\">" << lines_included << "</td>" << std::endl;
    out << "<td style=\"text-align:right;\">" << lines_covered << "</td>" << std::endl;
    out << "<td style=\"text-align:right;\">" << lines_uncovered << "</td>" << std::endl;
    out << "<td style=\"text-align:right;box-shadow: -8px 0 0 0 #" << lines_color << "dd inset;\">" << std::fixed << std::setprecision(1) << lines_percent << "%</td>" << std::endl;
    out << "</tr>" << std::endl;
  }
}

void MarkdownGenerator::coverage_foot(const Entity& entity,
    const Entity& root, std::ofstream& out) {
  auto dirs = view(entity.dirs, true);
  for (auto& child : dirs) {
    coverage_foot(*child, root, out);
  }

  /* here data-parent is set to the name of the entity itself, not its parent,
   * as the summary row in the footer should be shown when the entity is
   * selected as the root */
  std::string name = sanitize(entity.path.string());
  uint32_t lines_included = entity.lines_included;
  uint32_t lines_covered = entity.lines_covered;
  uint32_t lines_uncovered = lines_included - lines_covered;
  double lines_percent = (lines_included > 0) ?
      100.0*lines_covered/lines_included : 0.0;
  std::string lines_color = color(lines_percent);
  std::string style = (name == root.path.string()) ? "" : ", style=\"display:none;\"";

  out << "<tr id=\"summary." << name << "\" data-parent=\"" << name << "\"" << style << ">" << std::endl;
  out << "<td style=\"text-align:left;font-weight:bold;\">Summary</td>" << std::endl;
  out << "<td style=\"text-align:right;font-weight:bold;\">" << lines_included << "</td>" << std::endl;
  out << "<td style=\"text-align:right;font-weight:bold;\">" << lines_covered << "</td>" << std::endl;
  out << "<td style=\"text-align:right;font-weight:bold;\">" << lines_uncovered << "</td>" << std::endl;
  out << "<td style=\"text-align:right;font-weight:bold;box-shadow: -8px 0 0 0 #" << lines_color << "dd inset;\">" << std::fixed << std::setprecision(1) << lines_percent << "%</td>" << std::endl;
  out << "</tr>" << std::endl;
}

void MarkdownGenerator::sunburst(const Entity& entity, const Entity& root,
    std::ofstream& out) {
  out <<
  R""""(
  <div style="position:relative;width:100%;padding-top:100%;">
    <div id="coverage-sunburst" style="position:absolute;top:0;left:0;width:100%;height:100%;"></div>
  </div>
  <script src="https://cdn.jsdelivr.net/npm/echarts@5.5.1/dist/echarts.min.js"></script>
  <script type="text/javascript">
  )"""";
  out << "var data = [";
  sunburst_data(entity, root, out);
  out << ']' << std::endl;
  out <<
  R""""(
  var coverage_root = "";  // current root of coverage report
  function update_coverage_table(params) {
    if (typeof params.data.name === 'undefined') {
      // occurs when the central circle is selected to go up one level
      var path = coverage_root.substring(0, coverage_root.lastIndexOf('/'));
      var is_dir = true;
    } else {
      var path = params.data.path;
      var is_dir = params.data.type === 'dir';
    }
    let rows = document.querySelectorAll('[data-parent]');
    if (is_dir) {
      for (let row of rows) {
        if (row.dataset.parent === path) {
          row.style.display = '';
        } else {
          row.style.display = 'none';
        }
      }
    } else {
      for (let row of rows) {
        if (row.id === path) {
          row.style.display = '';
        } else {
          row.style.display = 'none';
        }
      }
    }
    coverage_root = path;
  }

  var coverage_sunburst = echarts.init(document.getElementById('coverage-sunburst'));
  var option = {
    series: {
      type: 'sunburst',
      data: data,
      sort: null,
      radius: ['5%', '95%'],
      startAngle: 0,
      clockwise: false,
      itemStyle: {
        borderWidth: 1
      },
      label: {
        color: 'white',
        fontSize: 10,
        textBorderWidth: 1,
        align: 'center',
        rotate: 'radial',
        width: 80,
        minAngle: 4,
        overflow: 'truncate',
        formatter: function (params) {
          if (params.data.icon) {
            return params.name + '\n' + params.data.icon;
          } else {
            return params.name;
          }
        }
      },
      labelLayout: {
        hideOverlap: true
      },
      levels: [
        {
          itemStyle: {
            opacity: 0.2
          }
        }
      ]
    }
  };
  coverage_sunburst.setOption(option);
  coverage_sunburst.on('click', update_coverage_table);
  window.addEventListener("resize", () => {
    coverage_sunburst.resize();
  });
  </script>
  )"""";
}

void MarkdownGenerator::sunburst_data(const Entity& entity,
    const Entity& root, std::ofstream& out) {
  bool first = true;
  for (auto& dir: view(entity.dirs, true)) {
    double percent = (dir->lines_included > 0) ?
        100.0*dir->lines_covered/dir->lines_included : 0.0;
    std::string c = color(percent);
    std::string ico = icon(percent);
    std::string path = relative(dir->path, root.path);

    if (!first) {
      out << ", ";
    }
    first = false;

    out << '{';
    out << "name: \"" << dir->name << "\", ";
    out << "path: \"" << path << "\", ";
    out << "value: " << dir->lines_included << ", ";
    out << "type: \"dir\", ";
    out << "icon: \"" << ico << "\", ";
    out << "children: [";
    sunburst_data(*dir, root, out);
    out << "], ";
    out << "itemStyle: { color: \"#" << c << "dd\", borderColor: \"#" << c << "\"}, ";
    out << "label: { textBorderColor: \"#" << c << "\"}";
    out << '}';
  }
  for (auto& file: view(entity.files, true)) {
    double percent = (file->lines_included > 0) ?
        100.0*file->lines_covered/file->lines_included : 0.0;
    std::string c = color(percent);
    std::string ico = icon(percent);
    std::string path = relative(file->path, root.path);

    if (!first) {
      out << ", ";
    }
    first = false;

    out << '{';
    out << "name: \"" << file->name << "\", ";
    out << "path: \"" << path << "\", ";
    out << "value: " << file->lines_included << ", ";
    out << "type: \"file\", ";
    out << "icon: \"" << ico << "\", ";
    out << "itemStyle: { color: \"#" << c << "dd\", borderColor: \"#" << c << "\"}, ";
    out << "label: { textBorderColor: \"#" << c << "\"}";
    out << '}';
  }
}

bool MarkdownGenerator::can_write(const std::filesystem::path& path) {
  bool canWrite = true;
  if (std::filesystem::exists(path)) {
    try {
      YAMLParser parser(path.string());
      YAMLNode node = parser.parse();
      canWrite = node.isValue("generator") && node.value("generator") == "doxide";
    } catch (const std::runtime_error& e) {
      warn(e.what());
      canWrite = false;
    }
  }
  if (!canWrite) {
    warn(path.string() << " already exists and was not generated by doxide, will not overwrite");
  }
  return canWrite;
}

std::string MarkdownGenerator::relative(const std::filesystem::path& path,
    const std::filesystem::path& base) {
  if (base.empty()) {
    return path.string();
  } else {
    std::filesystem::path result = std::filesystem::relative(path, base);
    if (result == ".") {
      return "";
    } else {
      return result.string();
    }
  }
}

std::string MarkdownGenerator::frontmatter(const Entity& entity) {
  /* use YAML frontmatter to ensure correct capitalization of title, and to
   * mark as managed by Doxide */
  std::stringstream buf;
  buf << "---" << std::endl;
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
  static const std::regex word("\\w|[./\\\\]");
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

  const int base_red = 0xef5552;
  const int base_yellow = 0xffc105;
  const int base_green = 0x4cae4f;

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

std::string MarkdownGenerator::icon(const double percent) {
  assert(0.0 <= percent && percent <= 100.0);

  if (percent < 60.0) {
    return "○○○○";
  } else if (percent < 70.0) {
    return "●○○○";
  } else if (percent < 80.0) {
    return "●●○○";
  } else if (percent < 90.0) {
    return "●●●○";
  } else {
    return "●●●●";
  }
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
