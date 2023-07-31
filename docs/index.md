<style>
  .md-typeset h1,
  .md-content__button {
    display: none;
  }
</style>

<div style="text-align:center;">
<img src="assets/title.svg" width="256" height="384" alt="Doxide: Modern documentation for modern C++">
</div>

<div style="text-align:center;">
<p><a href="/demo" class="md-button md-button--primary">Demo</a>
<a href="/installation" class="md-button md-button--primary">Download</a></p>
</div>

Doxide is a documentation generator for C++.

* It is configured with YAML, generates Markdown, and publishes HTML with a modern look and responsive design for desktop and mobile devices.
* It is open source under an Apache 2.0 license, and runs on Linux, Mac, and Windows.
* It is written in C++ so that its primary users, C++ developers, can readily contribute.
* C++ source code is documented with `/** comments */` containing `@commands`, as with the classic tool [Doxygen](https://doxygen.nl/){target="_blank"}. Many commands from Doxygen are already supported.
* C++ source code is parsed with [Tree-sitter](https://tree-sitter.github.io){target="_blank"}, the same parser used by many syntax highlighters.


By generating Markdown, Doxide opens C++ documentation to the whole wide world of static site generation tools and themes. There is particular support for [MkDocs](https://www.mkdocs.org/){target="_blank"} and the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/){target="_blank"} theme, as on the Doxide website. A little extra effort enables alternatives such as [Jekyll](https://jekyllrb.com/){target="_blank"} and [Hugo](https://gohugo.io/){target="_blank"}. Other formats such as PDF are possible too, via [Pandoc](https://pandoc.org/){target="_blank"}.

This is all very new, a humble attempt to modernize documentation for the C++ community. Further features will come, but feedback is sought and contributions very welcome. See the [GitHub repository](https://github.com/lawmurray/doxide){target="_blank"} to report an issue or contribute a bug fix or feature, or contact Lawrence Murray ([:material-github:](https://github.com/lawmurray){target="_blank"} [:material-mastodon:](https://fosstodon.org/@lawmurray){target="_blank"} [:material-twitter:](https://twitter.com/lawmurray){target="_blank"} [:material-web:](https://indii.org){target="_blank"} [:material-email:](mailto:lawrence@indii.org){target="_blank"}).
