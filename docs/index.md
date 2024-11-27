---
description: Documentation and code coverage for modern C++. Configure with YAML, generate Markdown, publish with Material for MkDocs.
---

<style>
  .md-typeset h1,
  .md-content__button {
    display: none;
  }
</style>

<div style="text-align:center;">
<img src="assets/title.svg" width="256" height="384" alt="Doxide: Modern documentation for modern C++">
</div>

<div style="text-align:center;padding-bottom:1em;">
<a href="/installation" class="md-button md-button--primary">Get Started</a>
<a href="/demo" class="md-button md-button--primary">See Demo</a>
</div>

Doxide is a documentation generator for C++. It parses C++ source files annotated with documentation `/** comments */` or `/// comments` with additional `@commands` or arbitrary Markdown formatting. Doxide then publishes HTML documentation using [MkDocs](https://www.mkdocs.org/){target="_blank"} and the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/){target="_blank"} theme. This website is an example.

Doxide also generates [code coverage reports](coverage-reports.md) with sunburst charts and sortable tables, based on `gcov` data and source code parsing. These interactive reports make it easy to drill into a codebase and determine where further testing may be warranted to increase code coverage. The underlying approach improves the accuracy of code coverage results with C++, being aware of function templates that have not been instantiated, which are a blind spot of other tools.

* Doxide is configured with YAML, generates Markdown, and publishes HTML for the web.
* It aims at a modern look and feel, with responsive design for both desktop and mobile devices.
* It is written in C++ so that its primary users, C++ developers, can readily contribute.
* It is open source under an Apache 2.0 license, and runs on Linux, Mac, and Windows.
* It parses C++, with support for CUDA extensions, using [Tree-sitter](https://tree-sitter.github.io).
* To help with migration from other documentation tools, many standard [Doxygen](https://doxygen.nl) commands are supported.

Development of Doxide is active, feedback is sought, and contributions very welcome. See the [GitHub repository](https://github.com/lawmurray/doxide){target="_blank"} to report an issue or contribute a bug fix or feature, or contact Lawrence Murray ([:material-github:](https://github.com/lawmurray){target="_blank"} [:material-mastodon:](https://fosstodon.org/@lawmurray){target="_blank"} [:material-web:](https://indii.org){target="_blank"} [:material-email:](mailto:lawrence@indii.org){target="_blank"}).

=== "C++ Input"

    ```cpp
    /**
     * An example.
     * 
     * @tparam T Template parameter.
     * 
     * @param x Input parameter.
     * @param[out] y Output parameter.
     * 
     * @return Return value.
     * 
     * @tip There are plenty more [demonstrations available](/demo)!
     */
    template<class T>
    int f(int x, int& y);
    ```

=== "HTML Output"

    !!! function "template&lt;class T&gt; int f(int x, int&amp; y)"
        
        An example.

        :material-code-tags: `T`
        :    Template parameter.
        
        :material-location-enter: `x`
        :    Input parameter.
        
        :material-location-exit: `y`
        :    Output parameter.
        
        :material-keyboard-return: **Return**
        :    Return value.

        !!! tip
            There are plenty more [demonstrations available](/demo)!
