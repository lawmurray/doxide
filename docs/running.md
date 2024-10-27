# Running Doxide

Build the Markdown documentation with:
```
doxide build
```
This will populate the output directory (default: `docs`).

!!! tip
    You may see warning messages about parse errors. These are not necessarily fatal and may not affect the documentation at all. Some relate to the use of the preeprocessor and can be fixed by defining symbols in the configuration file, see [Parsing Considerations](parsing.md).

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
