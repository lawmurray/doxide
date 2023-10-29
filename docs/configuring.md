# Configuring Doxide

The `doxide.yaml` configuration file supports the following entries.

`title`
:   Main page title. Overruled by [command-line](command-line.md) option `--title`.

`description`
:   Main page description. Overruled by [command-line](command-line.md) option `--description`.

`output` (default `docs`)
:   Output directory. Overruled by [command-line](command-line.md) option `--output`.

`files`
:   List of source files from which to extract documentation. The following wildcards are supported for pattern matching:

    | Pattern | Description                                           |
    | ------- | ----------------------------------------------------- |
    | `?`     | Any single character.                                 |
    | `*`     | Zero or more characters, without directory recursion. |
    | `**`    | Zero or more characters, with directory recursion.    |
    | `[]`    | Any characters between the square brackets.           |
    | `[!]`   | Any characters *not* between the square brackets.     |
    | `[a-z]` | Any characters in the range `a` to `z`.               |

    For example, `*.hpp` will match all files in the root directory with a file extension of `.hpp`, while `**/*.hpp` will match all files in subdirectories, recursively, with a file extension of `.hpp`.

`groups`
:   List of groups used to [organize](organizing.md) documentation. Each element of the list can further contain the keys:

    `name`
    : Name of the group. This is used to assign entities to the group using the `@ingroup` command.

    `title`
    : Title of the group, used for display purposes.

    `description`
    : Description of the group, used for display purposes.

    `groups`
    : Further subgroups, following the same schema.

`defines`
:   Mapping of preprocessor symbol definitions, where keys are the symbols
    and values are their replacements. This may be used to troubleshoot [parsing](parsing.md) issues related to the use of preprocessor macros.

!!! example

    ```yaml
    title: Example
    description: Example configuration file.
    output: docs
    files:
      - header.hpp
      - "*/*.hpp"  # quotes may be needed when using wildcards or other special characters
      - "**/*.hpp"
    defines:
      SYMBOL: value
      DEFINED_BUT_EMPTY_SYMBOL: ""      
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