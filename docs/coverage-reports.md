# Understanding Code Coverage Reports

!!! tip
    See the [demonstration](/demo/coverage) for an example code coverage report.

!!! warning
    This is a new and somewhat experimental feature and behavior is subject to change. Please [report problems](https://github.com/lawmurray/doxide/issues).

Once [enabled](coverage.md), Doxide will include a code coverage report in the documentation. This is linked from the front page, after [groups](organizing.md) and namespaces. The report consists of three components:

1. an [interactive sunburst chart](#interactive-sunburst-chart),
2. a [sortable table](#sortable-table) of source files and directories, and
3. [source code listings](#source-code-listings) of all source files with annotated lines.

Doxide uses source code to determine which lines are included in these code coverage reports. The approach aims to address a limitation in other tools where some source code is invisible, especially uninstantiated function templates, so that code coverage percentages are overestimated. This occurs when including lines from the compiled binary only, as some source code, such as that of uninstantiated function templates, never makes it into the compiled binary.

![Venn diagram depicting how Doxide computes coverage results: number of covered lines as derived from the binary program divided by number of included lines as derived from the source](/assets/venn_coverage.svg)

The Venn diagram above illustrates Doxide's definition of code coverage. Doxide reports the percentage of covered lines within a set of included lines as derived from source code (the area of the shaded region as a percentage of the area of the circle labeled *source-derived included lines*). This is different from tools that report the percentage of covered lines within a set of included lines as derived from the compiled binary (the area of the circle labeled *binary-derived covered lines* as a percentage of the area of the circle labeled *binary-derived included lines*). Doxide's approach may both include lines that are not visible in the binary (such as inline code and uninstantiated function templates), and exclude lines that are visible in the binary (usually uninteresting from the programmer's perspective).

This is not perfect. False-inclusions and false-exclusions do occur in Doxide code coverage reports. To some extent this reflects unresolvable ambiguities in what should and should not be included, and perhaps disagreement over what constitutes code coverage. Or it could be a fixable issue ([please report](https://github.com/lawmurray/doxide/issues)).

## Interactive sunburst chart

The sunburst chart visualizes all source files according to the file hierarchy. The innermost ring represents the top level directory (or directories). Each slice represents a directory or file. Each directory slice is further divided into its subdirectories and files in the next ring out.

The angle subtended by a slice is proportional to the number of included lines that it contains with respect to the total number of included lines at that level of the file hierarchy. For a file it is the number of included lines in the file (this is less than or equal to the total number of lines in the file). For a directory it is the number of included lines in all files and subdirectories below it (recursively). Each ring can therefore be interpreted as a pie chart at that level of the file hierarchy.

A color and marker is assigned to each slice according to the percentage of lines covered. For a file it is the percentage of lines covered in that file. For a directory it is the percentage of lines covered in all files and subdirectories below it (recursively). The colors and markers are:

<table>
<tr>
  <td style="background-color:#4cae4fdd;border-color:#4cae4f;color:white">●●●●</td>
  <td>90-100% of lines covered</td>
</tr>
<tr>
  <td style="background-color:#a5b72add;border-color:#a5b72a;color:white">●●●○</td>
  <td>80-90% of lines covered</td>
</tr>
<tr>
  <td style="background-color:#ffc105dd;border-color:#ffc105;color:white">●●○○</td>
  <td>70-80% of lines covered</td>
</tr>
<tr>
  <td style="background-color:#f78b2bdd;border-color:#f78b2b;color:white">●○○○</td>
  <td>60-70% of lines covered</td>
</tr>
<tr>
  <td style="background-color:#ef5552dd;border-color:#ef5552;color:white">○○○○</td>
  <td>0-60% of lines covered</td>
</tr>
</table>

Clicking on a slice selects that file or directory as the current and redraws the chart with that file or directory at the innermost ring. Clicking in the center of the chart selects the parent directory as the current and redraws the chart with that parent directory at the innermost ring. Updates are also reflected in the table beneath the chart.

## Sortable table

The table below the sunburst chart gives more detailed information about the current selection. It updates with the current selection of the sunburst chart. Clicking on any of the column headings will sort the rows in ascending or descending order by that column. This allows, for example, sorting in descending order by the number of uncovered lines, which may help in targeting further tests to increase overall coverage.

Selecting a file or directory from the table will load a new page. For a directory that page contains the same components as the original but focused on the selected directory only. For a file it brings up the source code listing of that file.

## Source code listings

When a file is selected, a new page is loaded with its full source code. All lines are numbered down the left. Line numbers are annotated with the following:

<table>
<tr>
  <td style="text-align:right;background-color:#4cae4fdd;border-color:#4cae4f;color:white"><tt>● 101</tt></td>
  <td>Line 101 is covered</td>
</tr>
<tr>
  <td style="text-align:right;background-color:#ef5552dd;border-color:#ef5552;color:white"><tt>○ 101</tt></td>
  <td>Line 101 is uncovered</td>
</tr>
<tr>
  <td style="text-align:right;"><tt>101</tt></td>
  <td>Line 101 is not included</td>
</tr>
</table>

You may notice inaccuracies in these classifications, e.g. lines that are excluded that perhaps should not be, or lines that are included but perhaps should not be. To some extent these may represent technical limitations, but consider [reporting an issue](https://github.com/lawmurray/doxide/issues) if there appears to be a consistent pattern that could be resolved.

!!! info
    For further reading:
    
     * [C++ Code Coverage with Gcov, Gcovr, and Doxide](https://indii.org/blog/cplusplus-code-coverage-with-gcov-gcovr-doxide/) for the thinking behind Doxide's code coverage support.

