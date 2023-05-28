# Q & A

## Why the name?

The [Doxygen](https://doxygen.nl) portmanteau is worthy of a riff. An oxide is a molecule containing an oxygen atom.

## What does the logo represent?

The logo is an oxygen atom from a chemistry model kit, the two holes being where you would connect it to other atoms to form a molecule (an oxide!). It is deliberately oriented to look like the face of some cute character.

## Does Doxide support programming languages other than C++?

Not yet, but it very possibly can. It almost certainly works for C already, although is untested in that regard, and some tweaks to the presentation may be desirable. For other programming languages, if they can be parsed with [Tree-sitter](https://tree-sitter.github.io/tree-sitter/), support is possible in principle, and contributions are welcome.

## What are some alternatives to Doxide?

The advantages of Doxide are a modern look, YAML configuration, Markdown output, and accurate parsing of C++ with [Tree-sitter](https://tree-sitter.github.io). If this does not appeal, there are some other alternatives, also open source:

1. [Doxygen](https://doxygen.nl/). It's an institution. Highly configurable. The downside is that the output might be considered dated.
2. [Doxygen Awesome](https://jothepro.github.io/doxygen-awesome-css/), which provides a fresh look for Doxygen via an alternative style sheet.
3. The combination of [Doxygen](https://doxygen.nl/) + [Breathe](https://www.breathe-doc.org/) + [Sphinx](https://www.sphinx-doc.org/en/master/), which also provides a fresh look, by taking the XML output of Doxygen, converting it to reStructuredText with Breathe, then converting it to HTML with Sphinx.
4. [MrDox](https://github.com/cppalliance/mrdox), which aims to be a replacement for Doxygen, using the clang parser.
