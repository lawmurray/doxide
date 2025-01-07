# Enabling Code Coverage

!!! warning
    This is a new and somewhat experimental feature and behavior is subject to change. Please [report problems](https://github.com/lawmurray/doxide/issues).

Doxide can produce interactive code coverage reports for C++ code, including which lines have, and have not, been executed. These can be used to drill down into source files with poor coverage that may warrant additional testing.

Doxide code coverage reports include a sunburst chart, tables sortable by column, and line-by-line annotated source code. They are constructed through a combination of source code analysis and binary program instrumentation. The combination is intended to provide more accurate coverage reports for C++ codebases, especially by accounting for uninstantiated function templates that are are blind spot for other tools, and which can produce misleading coverage results.

The basic workflow to enable code coverage is:

1. Compile code (e.g. test suite) with instrumentation for code coverage.
2. Run code to collect data on which lines are executed.
3. Summarize the code coverage data into a single file with command-line tools.
4. Run Doxide to produce the final report.

## Compiling for Code Coverage

The following instructions work for both GCC and LLVM. Add the following options when compiling with `g++` or `clang++`:
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

After compiling with the above options, simply run the program (e.g. test suite) as normal. The instrumentation will create additional `.gcda` files alongside each `.gcno` file created in the previous step. These files contain data on how many times each line of the program was executed. The data accumulates across runs, so you can run the code multiple times, perhaps in different configurations, to accumulate coverage data.


## Collating Code Coverage Data

The next step is to collate all the `.gcno` and `.gcda` files into one file that can be ingested by Doxide. The recommended approach is to use `gcov`, which is included with GCC, or `llvm-cov gcov`, which is included with LLVM. For the former:
```
find . -name '*.gcda' | xargs gcov --stdout > coverage.gcov
```
For the latter, just replace `gcov` with `llvm-cov gcov`:
```
find . -name '*.gcda' | xargs llvm-cov gcov --stdout > coverage.gcov
```
Either of these commands creates a `coverage.gcov` file that can be passed to `doxide` in the next step.

### Alternatives

The recommended approach is to create a `.gcov` file as above. An alternative, however, is to use JSON. The simplest way to do so is with [gcovr](https://gcovr.com):
```
gcovr --json coverage.json
```
This `coverage.json` file can then be used in place of the `coverage.gcov` file above. The only downside of this is that it introduces `gcovr` as an additional dependency in the workflow, which needs to be installed separately from GCC or LLVM.

It is also possible to export JSON from `gcov` directly, but this produces one JSON file (compressed with `gzip`) for each `.gcda` input file. Further postprocessing is then needed to decompress those files and massage them into a single `.json` file, perhaps using [jq](https://jqlang.github.io/jq/).


## Reporting Coverage Data

To enable code coverage reports with Doxide, provide it with the file produced in the previous step (e.g. `coverage.gcov`) when running `doxide build`. This can be done in one of two ways:

1. adding the `--coverage` [command-line option](command-line.md), e.g.
   ```
   doxide build --coverage coverage.gcov
   ```
   or
2. adding a `coverage` entry to the [configuration file](configuring.md), e.g.
   ```yaml
   title: Example
   description: Example configuration file.
   output: docs
   files:
   - "*/**.hpp"
   - "*/**.cpp"
   coverage: coverage.gcov
   ```

The former overrides the latter, and may be more convenient if code coverage reports are not routinely produced.

The standard `files` entry in the [configuration file](configuring.md) will be used to determine the set of files to include in the coverage report. This is the same entry as used for documentation. You may wish to add `.cpp` source files for the purposes of code coverage, which are not typically added for the purposes of documentation alone.


## Exporting Coverage Data

It is possible to export Doxide's code coverage data for further processing or use with other tools. Simply run:
```bash
doxide cover --coverage coverage.gcov > export.json
```
The `--coverage` option may be omitted if specified in the [configuration file](configuring.md) instead. This will output JSON to `export.json`. The JSON schema is the same as [that used by `gcovr`](https://gcovr.com/en/stable/output/json.html), a superset of [that used by `gcov`](https://gcc.gnu.org/onlinedocs/gcc/Invoking-Gcov.html#Invoking-Gcov). The data follows the definition of code coverage above. Lines that are not included by Doxide are not included in the data.

!!! info
    For further reading:
    
     * [C++ Code Coverage with Gcov, Gcovr, and Doxide](https://indii.org/blog/cplusplus-code-coverage-with-gcov-gcovr-doxide/) for the thinking behind Doxide's code coverage support.
     * [GCC Manual](https://gcc.gnu.org/onlinedocs/gcc/Gcov-Data-Files.html) for more information on `.gcno` and `.gcda` files.
