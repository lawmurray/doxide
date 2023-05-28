# Installation

!!! note
    Packages are on the way. In the meantime, it is necessary to install from source.

!!! note
    Currently Doxide only supports Linux and Mac. Windows support is on the way; in the meantime, it can be installed via Windows Subsystem for Linux.

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
