# Understanding Code Coverage Reports

!!! tip
    See the [demonstration](/demo/coverage) for an example code coverage report.

!!! warning
    This is a new and somewhat experimental feature and behavior is subject to change. Consider [reporting an issue](https://github.com/lawmurray/doxide/issues) if necessary.

Once [enabled](coverage.md), Doxide will include a code coverage reports in documentation, linked from the front page. These reports consist of three components:

1. interactive sunburst charts,
2. sortable tables, and
3. source code listings with annotated lines.

Doxide uses source code to determine which lines to include and exclude in these code coverage reports. This attempts to address a limitation in other tools based on the compiled binary, where some code, especially uninstantiated function templates, are invisible to the tool. This can result in overestimating code coverage.

![Venn diagram depicting how Doxide computes coverage results: number of covered lines as derived from the binary program divided by number of included lines as derived from the source](/assets/venn_coverage.svg)

The Venn diagram illustrates Doxide's definition of code coverage. It reports the percentage of covered lines within the set of included lines as derived from source code (the are of the shaded region as a percentage of the are of the *source-derived included lines* circle). This is different from tools that report the percentage of covered lines within the set of included lines as derived from the binary program (the area of the *binary-derived covered lines* circle as a percentage of the area of the *binary-derived included lines* circle). This may both include lines that are not visible in the binary (such as inline code and uninstantiated function templates), and exclude lines that are visible in the binary (usually uninteresting from the programmer's perspective).

This is not perfect. False-positive and false-negative lines do appear in Doxide code coverage reports. To some extent this reflects unresolvable ambiguities in what should and should not be included, and perhaps disagreement over what constitutes code coverage.

## Interactive sunburst charts

The sunburst chart visualizes all source files according to the file hierarchy. The innermost ring is the root directory (or perhaps several top-level directories). Each slice represents a directory or file. Each directory slice is further divided into its subdirectories and files in the next ring out.

The angle subtended by a slice is proportional to its number of lines with respect to the total number of lines at that level of the directory hierarchy. Each ring can therefore be interpreted as a pie chart at some level deep in the file hierarchy.

The color and icon associated with a slice is according to the percentage of lines covered, following the key:

<table>
<tr>
  <td style="background-color:#4cae4fdd;border-color:#4cae4f;color:white">●●●●</td>
  <td>90-100%</td>
</tr>
<tr>
  <td style="background-color:#a5b72add;border-color:#a5b72a;color:white">●●●○</td>
  <td>80-90%</td>
</tr>
<tr>
  <td style="background-color:#ffc105dd;border-color:#ffc105;color:white">●●○○</td>
  <td>70-80%</td>
</tr>
<tr>
  <td style="background-color:#f78b2bdd;border-color:#f78b2b;color:white">●○○○</td>
  <td>60-70%</td>
</tr>
<tr>
  <td style="background-color:#ef5552dd;border-color:#ef5552;color:white">○○○○</td>
  <td>0-60%</td>
</tr>
</table>

Clicking on a slice will redraw the chart with that file or directory as the root. Clicking in the centre of the chart will move up one directory and redraw the chart. These interactions will also update the table.

## Sortable tables

The table below the sunburst chart shows more detailed information about the files and directories immediately below the current root directory. It updates as the root directory changes with clicks on the sunburst chart. Clicking on any of the column headings will sort the rows in ascending or descending order by that column. This allows, for example, sorting in descending order by the number of uncovered lines, which may help in targeting further tests to increase overall coverage.

Selecting a file or directory from the table will load a new page. For a directory it is the same as the initial code coverage report but focused on that directory alone (it is not possible to navigate further up from here---go back to the previous page). For a file it brings up the source code listing of that file.

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
  <td>Line 101 is excluded</td>
</tr>
</table>

You may notice inaccuracies in these classifications, e.g. lines that are excluded that perhaps should not be, or lines that are included but perhaps should not be. To some extent these may represent technical limitations, but consider [reporting an issue](https://github.com/lawmurray/doxide/issues) if there appears to be a consistent pattern that could be corrected.

!!! info
    For further reading:
    
     * [C++ Code Coverage with Gcov, Gcovr, and Doxide](https://indii.org/blog/cplusplus-code-coverage-with-gcov-gcovr-doxide/) for the thinking behind Doxide's code coverage support.

