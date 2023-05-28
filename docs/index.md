---
title: Doxide
description: Modern documentation for modern C++. Configure with YAML and output Markdown.
---

<style>
  .md-typeset h1,
  .md-content__button {
    display: none;
  }
</style>

<div style="text-align:center;">
<img src="assets/title.svg" width="256" alt="Doxide: Modern documentation for modern C++">
</div>

Doxide generates documentation for C++ source code. It is configured with YAML, generates Markdown, and publishes HTML.

Entities in the source code are documented with special `/** comments */` containing `@commands`, as with the classic tool [Doxygen](https://doxygen.nl/){target="_blank"}. The source code is parsed and documentation processed into Markdown then HTML. Doxide aims at online documentation with a modern look and responsive design for desktop and mobile devices.

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
     */
    template<class T>
    int f(int x, int& y);
    ```

=== "Output"

    !!! function "template&lt;class T&gt; int f(int x, int&amp; y)"
        
        An example.

        :material-code-tags: **Template parameter** `T`
        :    Template parameter.
        
        :material-location-enter: **Parameter** `x`
        :    Input parameter.
        
        :material-location-exit: **Parameter** `y`
        :    Output parameter.
        
        :material-keyboard-return: **Return**
        :    Return value.


Doxide is open source software written in C++. It depends on [libyaml](https://pyyaml.org/wiki/LibYAML){target="_blank"} to parse YAML configuration files and [Tree-sitter](https://tree-sitter.github.io){target="_blank"} to parse source code. By generating Markdown, it opens a whole wide world of static site generation tools and themes. There is particular support for [MkDocs](https://www.mkdocs.org/){target="_blank"} and the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/){target="_blank"} theme. A little extra effort enables alternatives such as [Jekyll](https://jekyllrb.com/){target="_blank"} and [Hugo](https://gohugo.io/){target="_blank"}. Other formats such as PDF are possible too, via [Pandoc](https://pandoc.org/){target="_blank"}.

This is all very new, a humble attempt to modernize documentation for the C++ community. Further features will come, but feedback is sought and contributions very welcome. See the [GitHub repository](https://github.com/lawmurray/doxide){target="_blank"} to report an issue or contribute a bug fix or feature, or contact Lawrence Murray ([:material-github:](https://github.com/lawmurray){target="_blank"} [:material-mastodon:](https://fosstodon.org/@lawmurray){target="_blank"} [:material-twitter:](https://twitter.com/lawmurray){target="_blank"} [:material-web:](https://indii.org){target="_blank"} [:material-email:](mailto:lawrence@indii.org){target="_blank"}).
