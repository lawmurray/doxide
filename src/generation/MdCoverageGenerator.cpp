#include "MdCoverageGenerator.hpp"

#include <FileSystemDeleter.hpp>
#include <FileSystemStatsInfo.hpp>
#include <Sink.hpp>
#include <Utils.hpp>
#include <entities/FileEntity.hpp>
#include <entities/FwdRef.hpp>
#include <entities/LineStats.hpp>

#include "MdCommon.hpp"
#include "Sanitize.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <utility>

#include <iostream>

static constexpr std::string_view SUNBURST_PRELUDE = R""""(
<div id="coverage-sunburst" style="aspect-ratio:1/1;width:100%;max-width:600px;margin:auto;"></div>

<script type="text/javascript">
var data = [
)"""";

static constexpr std::string_view SUNBURST_POSTLUDE = R""""(]

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

window.addEventListener("load", () => {
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
});

window.addEventListener("load", () => {
  var tables = document.querySelectorAll("article table:not([class])");
  tables.forEach(function(table) {
    new Tablesort(table);
  });
});

</script>
)"""";


static constexpr std::string_view TABLE_COVERAGE_0 = R""""(
<table>
<thead>
<tr>
<th style="text-align:left;" data-sort-method="dotsep">Name</th>
<th style="text-align:right;" data-sort-method="number">Lines</th>
<th style="text-align:right;" data-sort-method="number">Covered</th>
<th style="text-align:right;" data-sort-method="number">Uncovered</th>
<th style="text-align:right;" data-sort-method="number">Coverage</th>
</tr>
</thead>
<tbody>
)"""";

static constexpr std::string_view TABLE_COVERAGE_1 = R""""(</tbody>
<tfoot>
)"""";

static constexpr std::string_view TABLE_COVERAGE_2 = R""""(</tfoot>
</table>
)"""";

static constexpr std::string_view material_file_outline = "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" fill=\"currentColor\" viewBox=\"0 0 24 24\"><path d=\"M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8zm4 18H6V4h7v5h5z\"/></svg>";

static constexpr std::string_view material_folder = "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"20\" height=\"20\" fill=\"currentColor\" viewBox=\"0 0 24 24\"><path d=\"M10 4H4c-1.11 0-2 .89-2 2v12a2 2 0 0 0 2 2h16a2 2 0 0 0 2-2V8a2 2 0 0 0-2-2h-8z\"/></svg>";

struct DisplayStats {
  double percent;
  std::string_view color;
  std::string_view icon;

  DisplayStats(const LineStats& s): percent(get_percent(s)),
  color(get_color(percent)), icon(get_icon(percent))
  {}

  static constexpr double get_percent(const LineStats& s) {
    return s.included_lines > 0 ? (100.0 * s.covered_lines) / s.included_lines : 100.0;
  }

  static inline constexpr std::string_view _get_value(
    const std::array<std::string_view, 5>& values, double percent) {
      assert(0.0 <= percent && percent <= 100.0);
      if (percent < 60.0) {
        return values[0];
      } else if (percent < 70.0) {
        return values[1];
      } else if (percent < 80.0) {
        return values[2];
      } else if (percent < 90.0) {
        return values[3];
      }
      return values[4];
  }

  static constexpr std::string_view get_color(double percent) {
    constexpr std::array<std::string_view, 5> colors = {
      "ef5552" /* red */, "f78b2b" /* orange */, "ffc105" /* amber */,
      "a5b72a" /* olive */, "4cae4f" /* green */
    };
    return _get_value(colors, percent);
  }

  static constexpr std::string_view get_icon(double percent) {
    constexpr std::array<std::string_view, 5> icons = {
      "○○○○", "●○○○", "●●○○", "●●●○", "●●●●"};
    return _get_value(icons, percent);
  }
};

template<typename T> struct DfsPrePostData {
  bool pre;
  T data;

  DfsPrePostData(bool _pre, T&& _data): pre(_pre), data(std::move(_data)) {}
};

template<typename T> DfsPrePostData<T> make_pre_dfs(T&& data) {
  return DfsPrePostData(true, std::move(data));
}

template<typename T> DfsPrePostData<T> make_post_dfs(T&& data) {
  return DfsPrePostData(false, std::move(data));
}

static void render_entry(sink_t& sink, const LineStats& stats, char letter, std::string_view root,
  std::string_view dirpath, std::string_view name, std::string_view material) {
  std::string parent = sanitize(
    FileStorage::relative_path(FileStorage::parent_path(dirpath), root));
  std::string sname = sanitize(name);
  DisplayStats ds(stats);
  std::string_view style = parent.empty() ? "" : " style=\"display:none;\"";
  std::string path = parent.empty() ? sname : parent + '/' + sname;
  std::size_t lines_uncovered = stats.included_lines - stats.covered_lines;

  sink.print(R""""(<tr id="{0}" data-parent="{1}"{2}>
<td style="text-align:left;" data-sort="{3}.{4}">{5} <a href="{0}/">{6}</a></td>
<td style="text-align:right;">{7}</td>
<td style="text-align:right;">{8}</td>
<td style="text-align:right;">{9}</td>
<td style="text-align:right;box-shadow: -8px 0 0 0 #{10}dd inset;">{11:.1f}%</td>
</tr>
)"""", path, parent, style, letter, sname, material, htmlize(sname),
    stats.included_lines, stats.covered_lines, lines_uncovered, ds.color, ds.percent
  );
}

struct CoverageDfs: public ConstDirStackController {
  using stack_value = ConstDirStackController::stack_value;
  using yield_value = DfsPrePostData<ConstDirStackController::yield_value>;

  yield_value make_yield_pre(const stack_value& top) {
    return make_pre_dfs(
      ConstDirStackController::yield_value{std::get<0>(top), std::get<1>(top)});
  }

  yield_value make_yield_post(const stack_value& top) {
    return make_post_dfs(
      ConstDirStackController::yield_value{std::get<0>(top), std::get<1>(top)});
  }
};

struct DirCovInfo {
  const FileSystemStatsInfo& fsinfo;
  sink_t& sink;
  std::string_view dirpath;
  const FileSystemStatsInfo::DirInfo& dirinfo;
  bool is_not_root;
};

static void coverage_data(DirCovInfo& info) {
  std::string_view root = info.is_not_root ? info.dirpath : "";
  CoverageDfs controller{info.fsinfo, info.dirpath};
  for (const auto& dirdata: dag_dfs_traverse<DfsTraversalType::BOTH>(controller)) {
    const auto& [dirpath, dirinfo] = dirdata.data;
    if (dirdata.pre) {
      if (dirpath == info.dirpath) {
        continue;
      }
      render_entry(info.sink, dirinfo.stats, 'a', root, dirpath,
        FileStorage::get_filename(dirpath), material_folder);
    } else {
      for(const auto& fe: dirinfo.files) {
        render_entry(info.sink, *fe, 'b', root, fe->get_path(),
          fe->get_filename(), material_file_outline);
      }
    }
  }
}

static void coverage_foot(DirCovInfo& info) {
  std::string_view root = info.is_not_root ? info.dirpath : "";
  ConstDirStackController controller{info.fsinfo, info.dirpath};
  for (const auto& [dirpath, dirinfo]: dag_dfs_traverse<DfsTraversalType::POST>(controller)) {
    std::string_view path = dirpath != FileSystemStatsInfo::DEF_ROOT_PATH ? dirpath : "";
    std::string name = sanitize(path);
    std::string_view style = (name == root) ? "" : " style=\"display:none;\"";
    const auto& stats = dirinfo.stats;
    DisplayStats ds(stats);
    std::size_t lines_uncovered = stats.included_lines - stats.covered_lines;

    info.sink.print(R""""(<tr id="summary.{0}" data-parent="{0}"{1}>
<td style="text-align:left;font-weight:bold;">Summary</td>
<td style="text-align:right;font-weight:bold;">{2}</td>
<td style="text-align:right;font-weight:bold;">{3}</td>
<td style="text-align:right;font-weight:bold;">{4}</td>
<td style="text-align:right;font-weight:bold;box-shadow: -8px 0 0 0 #{5}dd inset;">{6:.1f}%</td>
</tr>
)"""",
      name, style, stats.included_lines, stats.covered_lines,
      lines_uncovered, ds.color, ds.percent);
  }
}

struct SubdirCovDfs: public ConstDirStackController {
  using coverage_info_t = std::tuple<double, bool>;
  struct SunburstDfsStack: public ConstDirStackController::stack_value {
    coverage_info_t coverage_info;
  };

  using stack_value = SunburstDfsStack;
  using yield_value = DfsPrePostData<decltype(std::tuple_cat(
      std::declval<ConstDirStackController::yield_value>(),
      std::declval<coverage_info_t>()
  ))>;

  stack_value make_init() {
    auto initval = ConstDirStackController::make_init();
    double coverage = DisplayStats::get_percent(std::get<1>(initval).stats);
    return {std::move(initval), {coverage, false}};
  }

  stack_value make_next(stack_value& top) {
    auto next = ConstDirStackController::make_next(top);
    double perc = DisplayStats::get_percent(std::get<1>(next).stats);
    bool is_first = std::get<2>(next) == std::get<1>(next).subdirs.cbegin();
    return {std::move(next), {perc, is_first}};
  }

  yield_value make_yield_pre(const stack_value& top) {
    return make_pre_dfs(std::tuple_cat(ConstDirStackController::make_yield_pre(top),
      top.coverage_info));
  }

  yield_value make_yield_post(const stack_value& top) {
    return make_post_dfs(std::tuple_cat(ConstDirStackController::make_yield_pre(top),
      top.coverage_info));
  }
};

static void render_sunburst_preamble(DirCovInfo& covinfo,
  const std::string_view& dirpath, const FileSystemStatsInfo::DirInfo& dirinfo,
  double coverage_percentage, bool is_first_subdir) {
  static constexpr std::string_view SUNBURST_CHILDREN_PREAMBLE =
    "{{name: \"{}\", path: \"{}\", value: {},"
      " type: \"dir\", icon: \"{}\", children: [";
  if (dirpath == covinfo.dirpath) {
    return;
  }
  if (not is_first_subdir) {
    covinfo.sink.print(", ");
  }
  std::string_view relative_path = covinfo.is_not_root ?
    dirpath.substr(covinfo.dirpath.size() + 1) : dirpath;
  std::string_view dirname = FileStorage::get_filename(relative_path);
  std::string_view icon = DisplayStats::get_icon(coverage_percentage);
  covinfo.sink.print(SUNBURST_CHILDREN_PREAMBLE, dirname, relative_path,
    dirinfo.stats.included_lines, icon
  );
}

static void render_sunburst_postamble(DirCovInfo& covinfo,
  const std::string_view& root,
  const FileSystemStatsInfo::DirInfo& dirinfo, double coverage_percentage) {
  static constexpr std::string_view SUNBURST_CHILDREN_POSTAMBLE =
    "], itemStyle: {{ color: \"#{0}dd\", borderColor: \"#{0}\"}}, "
    "label: {{ textBorderColor: \"#{0}\"}}"
    "}}";

  static constexpr std::string_view SUNBURST_FILE_COVERAGE_INFO =
    "{{"
    "name: \"{0}\", path: \"{1}\", value: {2}, type: \"file\", icon: \"{3}\", "
    "itemStyle: {{ color: \"#{4}dd\", borderColor: \"#{4}\"}}, "
    "label: {{ textBorderColor: \"#{4}\"}}"
    "}}";

  std::string_view color = DisplayStats::get_color(coverage_percentage);
  bool first = dirinfo.subdirs.empty();
  if (not first) {
    covinfo.sink.print(SUNBURST_CHILDREN_POSTAMBLE, color);
  }
  std::size_t rel_beginning = not root.empty() ? root.size() + 1 : 0;
  for(const auto& fe: dirinfo.files) {
    if (not first) {
      covinfo.sink.print(", ");
    }
    DisplayStats ds(*fe);
    std::string_view relative_path = fe->get_path().substr(rel_beginning);
    first = false;
    covinfo.sink.print(SUNBURST_FILE_COVERAGE_INFO, fe->get_filename(), relative_path,
      fe->get_included_lines(), ds.icon, ds.color);
  }
}

static void sunburst(DirCovInfo& covinfo) {
  covinfo.sink.print("{}", SUNBURST_PRELUDE);
  std::string_view root = covinfo.is_not_root ? covinfo.dirpath : "";
  SubdirCovDfs controller{covinfo.fsinfo, covinfo.dirpath};
  for (const auto& dirdata:
    dag_dfs_traverse<DfsTraversalType::BOTH>(controller)) {
    const auto& [dirpath, dirinfo, coverage_percentage, is_first_subdir] = dirdata.data;
    if (dirdata.pre) {
      render_sunburst_preamble(covinfo, dirpath, dirinfo, coverage_percentage, is_first_subdir);
    } else {
      render_sunburst_postamble(covinfo, root, dirinfo, coverage_percentage);
    }
  }
  covinfo.sink.print("{}", SUNBURST_POSTLUDE);
}

static void render_dir_coverage(DirCovInfo& covinfo, std::string_view title) {
  covinfo.sink.println(FRONTMATTER);
  covinfo.sink.println("# {}\n", title);

  sunburst(covinfo);
  covinfo.sink.print(TABLE_COVERAGE_0);
  coverage_data(covinfo);
  covinfo.sink.print(TABLE_COVERAGE_1);
  coverage_foot(covinfo);
  covinfo.sink.print(TABLE_COVERAGE_2);
  covinfo.sink.println();
}

static void render_file_coverage(FileSystemDeleter& fscleaner,
  const std::filesystem::path& output_root,
  const FwdRef<FileEntity>& file) {
  static constexpr std::string_view covered_color = DisplayStats::get_color(100.0);
  static constexpr std::string_view uncovered_color = DisplayStats::get_color(0.0);
  std::filesystem::path outpath = output_root / file->get_path() / "";
  std::filesystem::create_directories(outpath.parent_path());
  std::filesystem::path outfile = outpath / "index.md";
  if (not can_write(outfile)) {
    return;
  }
  fscleaner.add_file(outfile);
  std::ofstream out(outfile);
  sink_t sink(out);

  /* for a file, output the whole contents; lines are highlighted with
   * CSS; two styles are given, one that works with MkDocs output, one
   * that works with pandoc output */

  sink.println(FRONTMATTER);
  sink.println("# {}\n", FileStorage::get_filename(file->get_name()));
  sink.println("{}\n", file->get_docs());
  /* for MkDocs, line numbers are added with the Markdown notation
   * `{linenums="1"}` below, which ultimately creates a HTML <table> of
   * one row <tr>, with two cells <td>, the first holding the line
   * numbers and the second the code; the line number cell contains a
   * further <pre> with one empty <span></span>, then each line number
   * in a separate <span> within it; to indicate code coverage we apply
   * styles to those <span>, which are readily selected by number using
   * the CSS nth-child() pseudo-class */
  sink.println(R""""(<style>
.linenodiv pre span {{padding-left:4px;padding-right:4px}})"""");

  const auto& line_counts = file->get_line_counts();
  for (std::size_t line = 0; line < line_counts.size(); ++line) {
    if (line_counts[line] < 0) {
      continue;
    }

    const bool pos = line_counts[line] > 0;
    const std::string_view color = pos ? covered_color : uncovered_color;
    const std::string_view dot = pos ? "●" : "○";
    sink.println(
      R""""(.linenodiv pre span:nth-child({0}) {{background-color: #{1}dd;color: white;}}
.linenodiv pre span:nth-child({0})::before {{content: "{2} ";}})"""",
      line + 2, color, dot);
  }

  /* for pandoc, line numbers are added through the CSS; there is a <span>
   * per line that is readily selectable with nth-child(), and it contains
   * an <a> at the start that is expanded to add the line number and
   * colour it */
  sink.println(R""""(.sourceCode pre span a {{padding-left:8px;padding-right:8px;text-decoration:none;}})"""");
  for (std::size_t line = 0; line < line_counts.size(); ++line) {
    const bool bigger = line_counts[line] > 0;
    const bool zero = line_counts[line] == 0;
    std::string_view dot = bigger ? "●" : (zero ? "○" : " ");
    sink.print(".sourceCode pre span:nth-child({}) a {{", line + 1);
    if (bigger or zero) {
      std::string_view color = bigger ? covered_color : uncovered_color;
      sink.print("background-color: #{}dd;", color);
    }
    const std::string_view color = (bigger or zero) ? "white" : "currentColor";
    sink.println(R""""(color: {};}}
.sourceCode pre span:nth-child({}) a::before {{content: "{} {:4}";}})"""",
      color, line + 1, dot, line);
  }
  sink.println(
R""""(</style>

```cpp {{linenums="1"}}
{}
```
)"""", file->get_content());
}

struct CovFilesRenderDfs: public ConstDirStackController {
  using stack_value = ConstDirStackController::stack_value;
  using yield_value = DfsPrePostData<ConstDirStackController::yield_value>;

  yield_value make_yield_pre(const stack_value& top) {
    return make_pre_dfs(ConstDirStackController::make_yield_pre(top));
  }

  yield_value make_yield_post(const stack_value& top) {
    return make_post_dfs(ConstDirStackController::make_yield_pre(top));
  }
};

void MdCoverageGenerator::generate(const FileSystemStatsInfo& fsinfo) {
  CovFilesRenderDfs controller{fsinfo};
  for (const auto& dirdata: dag_dfs_traverse<DfsTraversalType::BOTH>(controller)) {
    const auto& [dirpath, dirinfo] = dirdata.data;
    if (dirdata.pre) {
      std::cout << "-- dir: " << dirpath << std::endl;
      const bool is_not_root = dirpath != FileSystemStatsInfo::DEF_ROOT_PATH;
      std::filesystem::path outpath = is_not_root ?  output_root / dirpath : output_root;
      std::filesystem::create_directories(outpath);
      std::filesystem::path file = outpath / "index.md";
      if (not can_write(file)) {
        continue;
      }
      std::ofstream out(file);
      fscleaner.add_file(file);
      sink_t sink(out);
      std::string_view title = is_not_root?
        FileStorage::get_filename(dirpath) : "Code Coverage";
      DirCovInfo info{fsinfo, sink, dirpath, dirinfo, is_not_root};
      render_dir_coverage(info, title);
    } else {
      for(const auto& fe: dirinfo.files) {
        std::cout << "-- file: " << fe->get_path() << std::endl;
        render_file_coverage(fscleaner, output_root, fe);
      }
    }
  }
}
