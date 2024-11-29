# Command-line reference

Commands are:

`doxide init`
:   Initialize configuration files. Will prompt before replacing any existing files.

`doxide build`
:   Build documentation in the output directory.

`doxide watch`
:   Watch the documentation's source files and rebuild it on changes.

`doxide clean`
:   Clean the output directory.

`doxide cover`
:   Output [code coverage](coverage.md) data to `stdout` in JSON format.

`doxide help`
:   Print usage information.

Command-line options are as follows. When provided, they override the same settings in the [configuration file](configuring.md).

`--title` (default `Untitled`)
:   Main page title.

`--description`
:   Main page description.

`--output` (default `docs`)
:   Output directory.

`--coverage`
:   Code coverage file (`.gcov` or `.json`).
