# Code Coverage

!!! warning
    This is an experimental feature of Doxide that is still under development. Behavior is subject to change. See the blog post [C++ Code Coverage with Gcov, Gcovr, and Doxide](https://indii.org/blog/cplusplus-code-coverage-with-gcov-gcovr-doxide/) for more on the motivation and direction.

Doxide can provide line data to contribute to more accurate code coverage reports. This source-code-derived data supplements binary-program-derived data from code coverage tools such as `gcov`. It is particularly useful for including function templates in code coverage reports, even when not instantiated in a compile; these are often omitted by other tools.

To produce line data, simply run:
```bash
doxide cover
```
This will output JSON to `stdout`. The JSON is compatible with `gcov` and `gcovr` for producing code coverage reports. It provides information on all source files given in the [configuration file](configuring.md), indicating those lines occupied by function and operator bodies (i.e. executable code), including function template bodies. These are all given zero execution counts. This can then be merged with a JSON report from `gcov` or `gcovr` that provides nonzero counts after program execution. This helps ensure that all source code lines are included in the denominator when computing code coverage percentages, and that uninstantiated function templates are marked as uncovered, rather than merely omitted.

As this is still a developing feature, see the blog post [C++ Code Coverage with Gcov, Gcovr, and Doxide](https://indii.org/blog/cplusplus-code-coverage-with-gcov-gcovr-doxide/) for more information.

!!! tip
    For the purposes of code coverage, you may wish to add `*.cpp` source files, not just header files, to your [configuration file](configuring.md).
