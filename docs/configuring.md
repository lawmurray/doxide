# Configuring

The `doxide.yaml` configuration file supports the following entries.

`title`
:   Main page title. Overruled by [command-line](/command-line) option `--title`.

`description`
:   Main page description. Overruled by [command-line](/command-line) option `--description`.

`output` (default `docs`)
:   Output directory. Overruled by [command-line](/command-line) option `--output`.

`files`
:   List of source files from which to extract documentation. The wildcard `*` may be used to match multiple files, e.g. `*/*.hpp` will match all files in all subdirectories (non-recursively) that have a file extension of `.hpp`.

`groups`
:   List of groups used to [organize](/organizing) documentation. Each element of the list can further contain the keys:

    `name`
    : Name of the group. This is used to assign entities to the group using the `@ingroup` command.

    `title`
    : Title of the group, used for display purposes.

    `description`
    : Description of the group, used for display purposes.

    `groups`
    : Further subgroups, following the same schema.


!!! example

    ```yaml
    title: Example
    description: Example configuration file.
    output: docs
    files:
      - header.hpp
      - "*/*.hpp"  # quotes may be needed when using wildcards or other special characters
    groups:
      - name: top
        title: Top-level group
        description: Description of the top-level group.
        groups:
          - name: next
            title: Next-level group
            description: Description of the next-level group.
      - name: another
        title: Another top-level group.
        description: Demonstration of another top-level group.
    ```