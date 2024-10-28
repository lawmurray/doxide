description: A documentation generator for modern C++. Configure with YAML, generate Markdown, publish with Material for MkDocs.
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
<a href="/installation" class="md-button md-button--primary">Get Started</a></p>
</div>

Doxide is a documentation generator for modern C++. It consumes C++ sources annotated with `/** documentation comments */` containing `@special_commands`, generates Markdown, and publishes for the web. Doxide is an alternative to [Doxygen](https://doxygen.nl/){target="_blank"}, but aims at a more modern look and feel, with responsive design for desktop and mobile devices—just like this website.

* Doxide is configured with YAML, generates Markdown, and publishes HTML for the web.
* Doxide is written in C++ so that its primary users, C++ developers, can readily contribute. It uses [Tree-sitter](https://tree-sitter.github.io) to parse C++.
* Doxide is open source under an Apache 2.0 license, and runs on Linux, Mac, and Windows.
* To ease migration from other documentation tools, many standard Doxygen commands are supported.

=== "C++"

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
     * @tip There are plenty more demos [here](/demo).
     */
    template<class T>
    int f(int x, int& y);
    ```

=== "Output"

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
            There are plenty more demos [here](/demo).

By generating Markdown, Doxide opens C++ documentation to the whole wide world of static site generation tools and themes. There is particular support for [MkDocs](https://www.mkdocs.org/){target="_blank"} and the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/){target="_blank"} theme, as on this website. A little extra effort enables alternative static site generation tools such as [Jekyll](https://jekyllrb.com/){target="_blank"} and [Hugo](https://gohugo.io/){target="_blank"}. Other formats such as PDF are possible too, via [Pandoc](https://pandoc.org/){target="_blank"}.

This is all very new, a humble attempt to modernize documentation for the C++ community. Development is active, feedback is sought, and contributions very welcome. See the [GitHub repository](https://github.com/lawmurray/doxide){target="_blank"} to report an issue or contribute a bug fix or feature, or contact Lawrence Murray ([:material-github:](https://github.com/lawmurray){target="_blank"} [:material-mastodon:](https://fosstodon.org/@lawmurray){target="_blank"} [:material-web:](https://indii.org){target="_blank"} [:material-email:](mailto:lawrence@indii.org){target="_blank"}).
