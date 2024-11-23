# Enabling Code Coverage

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
    To add these for a CMake project, set the `CXXFLAGS` environment variable when calling `cmake` for the first time to configure your project, e.g.
    ```
    mkdir build
    cd build
    CXXFLAGS="--coverage -O0 -fno-inline -fno-elide-constructors" cmake -DCMAKE_BUILD_TYPE=Debug ..
    ```
    To add these for a GNU Autotools project, set the `CXXFLAGS` environment variable when calling the `configure` script to configure your project, e.g.
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


## Exporting Coverage Data

It is also possible to export Doxide's code coverage data for further processing or use with other tools. Simply run (omitting the `--coverage` option if specified in the [configuration file](configuring.md) instead):
```bash
doxide cover --coverage coverage.gcov > export.json
```
This will output JSON to `export.json`. The JSON schema is the same as [that used by `gcovr`](https://gcovr.com/en/stable/output/json.html), a superset of [that used by `gcov`](https://gcc.gnu.org/onlinedocs/gcc/Invoking-Gcov.html#Invoking-Gcov). The data follows the definition of code coverage above. Lines that are not included by Doxide are not included in the data.

!!! info
    For further reading:
    
     * [C++ Code Coverage with Gcov, Gcovr, and Doxide](https://indii.org/blog/cplusplus-code-coverage-with-gcov-gcovr-doxide/) for the thinking behind Doxide's code coverage support.
     * [GCC Manual](https://gcc.gnu.org/onlinedocs/gcc/Gcov-Data-Files.html) for more information on the `.gcno` and `.gcda` files.
