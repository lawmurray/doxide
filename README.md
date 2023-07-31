<p align="center">
<img src="docs/assets/title.svg" width="256" height="384" alt="Doxide: Modern documentation for modern C++">
</p>

Doxide is a documentation generator for C++.

* It is configured with YAML, generates Markdown, and publishes HTML with a modern look and responsive design for desktop and mobile devices.
* It is open source under an Apache 2.0 license, and runs on Linux, Mac, and Windows.
* It is written in C++ so that its primary users, C++ developers, can readily contribute.
* C++ source code is documented with `/** comments */` containing `@commands`, as with the classic tool [Doxygen](https://doxygen.nl/){target="_blank"}. Many commands from Doxygen are already supported.
* C++ source code is parsed with [Tree-sitter](https://tree-sitter.github.io){target="_blank"}, the same parser used by many syntax highlighters.


By generating Markdown, Doxide opens C++ documentation to the whole wide world of static site generation tools and themes. There is particular support for [MkDocs](https://www.mkdocs.org/){target="_blank"} and the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/){target="_blank"} theme, as on the Doxide website. A little extra effort enables alternatives such as [Jekyll](https://jekyllrb.com/){target="_blank"} and [Hugo](https://gohugo.io/){target="_blank"}. Other formats such as PDF are possible too, via [Pandoc](https://pandoc.org/){target="_blank"}.


## License

Doxide is open source software. It is licensed under the Apache License,
Version 2.0 (the "License"); you may not use it except in compliance with the
License. You may obtain a copy of the License at
<http://www.apache.org/licenses/LICENSE-2.0>.


## Installation

See the [website](https://doxide.org/installation) for packages for Linux, Mac, and Windows.

To install from source, clone the Doxide repo with:
```
git clone https://github.com/lawmurray/doxide --recurse-submodules
```
The `--recurse-submodules` is necessary to bring in some dependencies that are not universally available in package managers.

Build and install with:
```
cd doxide
cmake .
cmake --build .
cmake --install .
```
The last command may need `sudo`. Alternatively, provide a prefix to install
somewhere local:
```
cmake --install . --prefix $HOME/.local
```

Material for MkDocs is recommended for publishing your documentation. It is
distributed as a Python package. Install it with:
``` 
pip install mkdocs-material
```
For more information see [Material for
MkDocs](https://squidfunk.github.io/mkdocs-material/).


## Quick start

Run, from within your source code directory:
```
doxide init
```
This will create a `doxide.yaml` configuration file, as well as some
additional files for publishing with [Material for
MkDocs](https://squidfunk.github.io/mkdocs-material/). To start, it is not
necessary to modify any of these.

Build the Markdown:
```
doxide build
```
This will populate the output directory (default: `docs`).

Build the HTML:
```
mkdocs build
```
This will populate the `site` directory.

Serve the HTML:
```
mkdocs serve
```
(and point your browser to `localhost:8000`).


## Further information

For further information, see [doxide.org](https://doxide.org).
