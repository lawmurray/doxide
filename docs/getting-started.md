# Getting Started

## Initializing

To initialize Doxide files, use the following, typically in the root directory of your source code repository:

```bash
doxide init
```

Or, if you wish to specify some meta information:

```
doxide --name Example --version 0.0.0 --description "An example project."
```

This will produce the essential configuration file `doxide.yaml` for configuring Doxide. It will also produce the optional configuration file `mkdocs.yaml` for configuring MkDocs and Material for MkDocs, as well as a few theme files in the subdirectory `docs`.

Add at least `doxide.yaml` to version control, and the other files if you intend to use MkDocs and Material for MkDocs (highly recommended for a quick start---you can always try something else later).

## Configuring

The default `doxide.yaml` file is usually fine for a quick start. It will find all `*.hpp` and `*.h` files in any subdirectories. For details on its various options, see [Configurating documentation](/configuring-documentation).

Likewise, the default `mkdocs.yaml` is also fine for a quick start. It can be modified later to change the theme, colors, etc.

## Building

To build your documentation, use:
```
doxide build
```
This will populate the `docs` subdirectory with Markdown files.

## Publishing

To publish your documentation with MkDocs and Material for MkDocs, use:
```
mkdocs build
```
This will populate the `site` subdirectory with HTML, CSS, and JavaScript files. This directory can be published.

To serve the documentation locally, use:
```
mkdocs serve
```
and point your browser to the URL reported, usually `localhost:8000`.
