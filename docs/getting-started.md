# Starting

## Initializing

Run, from within your source code directory:
```
doxide init
```
This will create a `doxide.yaml` configuration file, as well as some
additional files for publishing with [Material for
MkDocs](https://squidfunk.github.io/mkdocs-material/). To start, it is not
necessary to modify any of these.

Add at least `doxide.yaml` to version control, and the other files if you intend to use Material for MkDocs (highly recommended for a quick start---you can always try something else later).


## Building

Build the Markdown documentation with:
```
doxide build
```
This will populate the output directory (default: `docs`).


## Publishing

To publish your documentation with Material for MkDocs, use:
```
mkdocs build
```
This will populate the `site` subdirectory with HTML, CSS, and JavaScript files. This directory can be published.

To serve the documentation locally, use:
```
mkdocs serve
```
and point your browser to the URL reported, usually `localhost:8000`.
