# Command-line reference

Commands are:

`doxide init`
:   Initialize configuration files. Will prompt before replacing any existing files.

`doxide build`
:   Build documentation in the output directory.

`doxide clean`
:   Clean the output directory.

`doxide cover`
:   Output (on `stdout`) zero-count line data for mixing with [code coverage](coverage.md) reports.

`doxide help`
:   Print usage information.

Command-line options are as follows. When provided, they override the same settings in the [configuration file](configuring.md).

`--title` (default `Untitled`)
:   Main page title.

`--description`
:   Main page description.

`--output` (default `docs`)
:   Output directory.
