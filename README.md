<p align="center">
<img src="docs/assets/title.svg" width="256" height="384" alt="Doxide: Modern documentation for modern C++">
</p>

Doxide is a documentation generator for C++.

* It is configured with YAML, generates Markdown, and publishes HTML with a modern look and responsive design for desktop and mobile devices.
* It is open source under an Apache 2.0 license, and runs on Linux, Mac, and Windows.
* It is written in C++ so that its primary users, C++ developers, can readily contribute.
* C++ source code is documented with `/** comments */` containing `@commands`, as with the classic tool [Doxygen](https://doxygen.nl/). Many commands from Doxygen are already supported.
* C++ source code is parsed with [Tree-sitter](https://tree-sitter.github.io), the same parser used by many syntax highlighters.


By generating Markdown, Doxide opens C++ documentation to the whole wide world of static site generation tools and themes. There is particular support for [MkDocs](https://www.mkdocs.org/) and the [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) theme, as on the Doxide website. A little extra effort enables alternatives such as [Jekyll](https://jekyllrb.com/) and [Hugo](https://gohugo.io/). Other formats such as PDF are possible too, via [Pandoc](https://pandoc.org/).

Instructions for installing from source are provided here. See the [website](https://doxide.org/installation) for packages for Linux, Mac, and Windows that are easier to install.


## License

Doxide is open source software. It is licensed under the Apache License,
Version 2.0 (the "License"); you may not use it except in compliance with the
License. You may obtain a copy of the License at
<http://www.apache.org/licenses/LICENSE-2.0>.


## Requirements

* A C++ toolchain for building.
* [CMake](https://cmake.org) for building.
* [LibYAML](https://pyyaml.org/wiki/LibYAML).
* Possibly [ICU](https://icu.unicode.org/), depending on your platform.

On Debian-based Linux systems, install these with:
```
sudo apt install cmake libyaml-dev 
```
On RPM-based Linux systems, install these with (replace `dnf` for Fedora with `zypper` for openSUSE, or otherwise for your distribution):
```
dnf install cmake libyaml-devel libicu-devel
```
On Mac systems using Homebrew, use:
```
brew install cmake libyaml
```
On Windows, build and install LibYAML separately from the `contrib/libyaml` directory.

There are further dependencies that do not need to be installed separately. They are included as submodules in the `contrib/` subdirectory either because they are not universally available in package managers or specific versions are required. The Doxide build handles these for you, but for reference:

* [CLI11](https://cliutils.github.io/CLI11/book/)
* [Tree-sitter](https://tree-sitter.github.io/tree-sitter/)
* [Tree-sitter C++ language support](https://github.com/tree-sitter/tree-sitter-cpp)
* [glob](https://github.com/p-ranav/glob)


## Installation

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

[Material for MkDocs](https://squidfunk.github.io/mkdocs-material/) is
recommended for publishing your documentation. It is distributed as a Python
package. Install it with:
``` 
pip install mkdocs-material
```


## Quick start

Run, from within your source code directory:
```
doxide init
```
This will create a `doxide.yaml` configuration file, as well as some
additional files for publishing with Material for MkDocs. To start, it is not
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
and point your browser to `localhost:8000`.


## Further information

For further information, see [doxide.org](https://doxide.org).
