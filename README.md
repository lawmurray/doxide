<p align="center">
<img src="docs/assets/title.svg" width="256" height="384" alt="Doxide: Modern documentation for modern C++">
</p>

Doxide generates documentation for C++ source code. It is configured with
YAML, generates Markdown, and publishes HTML.

Entities in the source code are documented with special `/** comments */`
containing `@commands`, as with the classic tool
[Doxygen](https://doxygen.nl/). The source code is parsed and documentation
processed into Markdown then HTML. Doxide aims at online documentation with a
modern look and responsive design for desktop and mobile devices.

Doxide is open source software written in C++. It depends on
[libyaml](https://pyyaml.org/wiki/LibYAML) to parse YAML configuration files
and [Tree-sitter](https://tree-sitter.github.io) to parse source code. By
generating Markdown, it opens a whole wide world of static site generation
tools. There is particular support for [MkDocs](https://www.mkdocs.org/) and
the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) theme.
A little extra effort enables alternatives such as
[Jekyll](https://jekyllrb.com/) and [Hugo](https://gohugo.io/). Other formats
such as PDF are possible too, via [Pandoc](https://pandoc.org/).


## License

Doxide is open source software. It is licensed under the Apache License,
Version 2.0 (the "License"); you may not use it except in compliance with the
License. You may obtain a copy of the License at
<http://www.apache.org/licenses/LICENSE-2.0>.


## Installation

Packages are on the way. Windows support is on the way. In the meantime, it is
necessary to install from source on Linux and Mac, and if that's you, use
Windows Subsystem for Linux.

Clone the Doxide source with:
```
git clone https://github.com/lawmurray/doxide --recurse-submodules
```
The `--recurse-submodules` is necessary to bring in the dependency
[tree-sitter-cpp](https://github.com/tree-sitter/tree-sitter-cpp).

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
