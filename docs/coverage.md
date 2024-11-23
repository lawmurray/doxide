# Code Coverage

!!! warning
    This is a new and somewhat experimental feature and behavior is subject to change. Consider [reporting an issue](https://github.com/lawmurray/doxide/issues) if necessary.

Doxide can produce interactive code coverage reports on which lines have been executed, and which have not, in a test suite. These can be used to drill down into source files with poor coverage to target future tests.

Doxide code coverage reports include a sunburst chart, tables sortable by column, and line-by-line annotated source code. They are constructed through a combination of source code parsing and binary program instrumentation. The combination is intended to provide more accurate coverage reports for C++ code, especially by accounting for uninstantiated function templates that are often ignored by other tools, which can be misleading with respect to code coverage.

The basic workflow for creating code coverage reports is as follows:

1. Compile code (e.g. test suite) with instrumentation for code coverage.
2. Run code to collect data on which lines are executed.
3. Summarize the code coverage data into a single file with command-line tools.
4. Run Doxide to produce the final report.

## Compiling for Code Coverage

The following instructions work for both GCC and LLVM. First, add the following options when calling `g++` or `clang++` when building your C++ code:
```
--coverage -O0 -fno-inline -fno-elide-constructors
```
The first, `--coverage`, is the most critical. It will create a `.gcno` file alongside each object file when compiling. The other options can improve code coverage reports somewhat by ensuring that optimizations do not inline functions in such a way that will make them invisible for the purposes of code coverage.

!!! tip
    To add these for a CMake project, set the `CXXFLAGS` environment variable before calling `cmake` for the first time to configure your project, e.g.
    ```
    mkdir build
    cd build
    CXXFLAGS="--coverage -O0 -fno-inline -fno-elide-constructors" cmake -DCMAKE_BUILD_TYPE=Debug ..
    ```
    To add these for a GNU Autotools project, set the `CXXFLAGS` environment variable before calling the `configure` script to configure your project, e.g.
    ```
    ./configure --prefix=$HOME/.local CXXFLAGS="--coverage -O0 -fno-inline -fno-elide-constructors"
    ```

## Running for Code Coverage

After compiling with code coverage enabled, simply run the test suite as normal. The instrumentation will create additional `.gcda` files alongside each `.gcno` file created in the previous step. These files contain data on how many times each line of the program was executed.


## Collating Code Coverage Data

The next step is to collate all the `.gcno` and `.gcda` files into one file that can be ingested by Doxide. The recommended approach is to use `gcov`, which is included with GCC:
```
find . -name '*.gcda' | xargs gcov --stdout > coverage.gcov
```
Alternatively, if using LLVM, use its `gcov` compatible tool `llvm-cov gcov` in place of `gcov`:
```
find . -name '*.gcda' | xargs llvm-cov gcov --stdout > coverage.gcov
```
Either of these commands creates a `coverage.gcov` file that can be passed to `doxide` in the next step.

### Alternatives

The recommended approach is to create a `.gcov` file as above. An alternative, however, is to use JSON. The simplest way to do so is with [gcovr](https://gcovr.com):
```
gcovr --json coverage.json
```
This `coverage.json` file can then be used in place of the `coverage.gcov` file above. The only downside of this is that it introduces `gcovr` as an additional dependency in the workflow.

It is also possible to export JSON from `gcov` directly, but this produces one JSON file (compressed with `gzip`) for each `.gcda` input file. Further postprocessing is then needed to decompress those files and merge them into a single `.json` file, perhaps using [jq](https://jqlang.github.io/jq/).


## Reporting Coverage Data

To enable code coverage reports with Doxide, simply provide it with the file produced in the previous step (e.g. `coverage.gcov`). This can be done in one of two ways:

 * adding a `--coverage` [command-line option](command-line.md) when calling `doxide`, e.g.
   ```
   doxide build --coverage coverage.gcov
   ```
   or
 * adding a `coverage` entry to the [configuration file](configuring.md), e.g.
   ```yaml
   title: Example
   description: Example configuration file.
   output: docs
   files:
   - "*/**.hpp
   - "*/**.cpp
   coverage: coverage.gcov
   ```

The former overrides the latter, and may be more convenient if code coverage reports are not routinely produced to silence warning messages of a non-existing coverage file.

The standard `files` entry in the [configuration file](configuring.md) will be used to determine the set of files to include in the coverage report. This is the same entry as used for documentation. You may wish to add `.cpp` source files for the purposes of code coverage, which are not typically added for the purposes of documentation alone.


## Understanding Code Coverage Reports

Doxide uses source code to determine which lines to include and exclude in its code coverage reports. This attempts to address a limitation in other tools based on the compiled binary, where some code, especially uninstantiated function templates that never make it to the compiled binary, are invisible to the tool. This can result in overestimating code coverage.

![Venn diagram depicting how Doxide computes coverage results: number of execu](/assets/venn_coverage.svg)

The Venn diagram illustrates Doxide's definition of code coverage. It reports the percentage of executed lines within the set of executable lines as derived from source code (the are of the shaded region as a percentage of the are of the *source-derived executable lines* circle). This is different from tools that report the percentage of executed lines within the set of executable lines as derived from the binary program (the area of the *binary-derived executed lines* circle as a percentage of the area of the *binary-derived executable lines* circle). This may both includes lines that are not visible in the binary (such as inline code and uninstantiated function templates), and exclude lines that are visible in the binary (usually uninteresting from the programmer's perspective).

This is not perfect. False-positive and false-negative lines do appear in Doxide code coverage reports. To some extent this reflects unresolvable ambiguities in what should and should not be included, and perhaps disagreement over what constitutes code coverage.

## Components of Code Coverage Reports

!!! tip
    See the [demonstration](/demo/coverage) for an example code coverage report.

Once enabled, Doxide will include a code coverage report in the generate documentation, linked form the front page. These reports consist of three components:

1. interactive sunburst charts,
2. sortable tables, and
3. source code listings with annotated lines.

### Interactive sunburst charts

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

### Sortable tables

The table below the sunburst chart shows more detailed information about the files and directories immediately below the current root directory. It updates as the root directory changes with clicks on the sunburst chart. Clicking on any of the column headings will sort the rows in ascending or descending order by that column. This allows, for example, sorting in descending order by the number of uncovered lines, which may help in targeting further tests to increase overall coverage.

Selecting a file or directory from the table will load a new page. For a directory it is the same as the initial code coverage report but focused on that directory alone (it is not possible to navigate further up from here---go back to the previous page). For a file it brings up the source code listing of that file.

### Source code listings

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

!!! tip
    You may notice inaccuracies in these classifications, e.g. lines that are excluded that perhaps should not be, or lines that are included but perhaps should not be. To some extent these may represent technical limitations, but consider [reporting an issue](https://github.com/lawmurray/doxide/issues) if there appears to be a consistent pattern that could be corrected.

## Exporting Coverage Data

It is also possible to export Doxide's code coverage data for further processing or use with other tools. Simply run (omitting the `--coverage` option if specified in the [configuration file](configuring.md) instead):
```bash
doxide cover --coverage coverage.gcov > export.json
```
This will output JSON to `export.json`. The JSON schema is the same as [that used by `gcovr`](https://gcovr.com/en/stable/output/json.html), a superset of [that used by `gcov`](https://gcc.gnu.org/onlinedocs/gcc/Invoking-Gcov.html#Invoking-Gcov). The data follows the definition of code coverage above. Lines that are not included by Doxide are not included in the data.


## Further Reading

* [C++ Code Coverage with Gcov, Gcovr, and Doxide](https://indii.org/blog/cplusplus-code-coverage-with-gcov-gcovr-doxide/) for the thinking behind Doxide's code coverage support.
* [GCC Manual](https://gcc.gnu.org/onlinedocs/gcc/Gcov-Data-Files.html) for more information on the `.gcno` and `.gcda` files.
