# Writing

Entities in C++ source code (e.g. classes, functions, operators, variables, macros) are documented by adding a *documentation comment* immediately preceding their declaration, typically in a header file. A documentation comment begins with `/**` rather than the usual `/*`, but ends with the usual `*/`. New lines may optionally start with an extra `*` character.

The documentation can be formatted with Markdown, and include *commands* using the syntax `@command`.

!!! example
    ```c++
    /**
     * An example class with documentation. It might include a list:
     * 
     * - first item,
     * - second item, and
     * - third item,
     *
     * or even a table:
     *
     * | Heading 1 | Heading 2 |
     * | --------- | --------- |
     * | Content 1 | Content 2 |
     */
    class Example {
      //
    };

    /**
     * An example function with documentation. This time we use commands.
     *
     * @param x First argument.
     * @param y Second argument.
     *
     * @return The result.
     */
    int f(int x, int y);
    ```


## Commands

The suggestion is to use Markdown wherever possible when writing documentation comments. Doxide provides a small set of commands that can be used to [organize](/organizing) documentation (e.g. `@ingroup`) and to ensure consistent formatting for common elements (e.g. `@param`).

| Command                                                      | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| `@param name`, `@param[in] name`, `@param[out] name`, `@param[in,out] name` | Document parameter `name`. The following paragraph is the description. The additional annotations `[in]`, `[out]` and `[in,out]` mark the parameter as an input (default), output or both input and output parameter. The following paragraph is the description. |
| `@tparam`                                                    | Document a template parameter `name`.                        |
| `@return`                                                    | Document the return value with the following paragraph.      |
| `@pre`, `@post`                                              | Document pre- or post-conditions with the following paragraph. |
| `@throw name`                                                | Document an exception `name` with the following paragraph.   |
| `@see`                                                       | Add a paragraph of "see also" references. The references themselves can be formatted in Markdown, using links if desired. |
| `@anchor name`                                               | Insert anchor that can be linked to from elsewhere with the Markdown syntax `[text](#name)`. |
| `@ingroup name`                                              | Add the entity to the group `name`. See [organizing](/organizing) for more information.        |
| `@@`                                                         | Escape: replaced with a single `@`.                                                               |
| `@/`                                                         | Escape: replaced with a single `/`.                                                               |

!!! tip
    As a particular use case, comments within documentation comments are possible by escaping the closing sequence `*/` as `*@/`. This can be useful when providing example code within a documentation comment.

## Migration support

To assist in the migration of existing code bases to Doxide from other documentation tools, some additional support is provided:

- The character `\` may be used as an alternative to `@` to denote commands. When `\` is used, unlike when `@` is used, no warning is given if the command is not found, and the command is output as-is. This is necessary to support LaTeX macros in mathematics without warning overload.

- The character `%` may be used as an escape for a single non-whitespace character (it is used to break automatic linking in Doxygen).
    
The following behaviors are implemented to assist in the migration of existing code bases. Recommended alternatives are provided for new code bases.

| Command(s)                                                   | Doxide behavior                               | Recommended alternative                                        |
| ------------------------------------------------------------ | --------------------------------------------- | ------------------------------------------------------------ |
| `@e word`, `@em word`, `@a word`                             | Replace with Markdown.                        | Use Markdown emphasis: `*word*`                              |
| `@b word`                                                    | Replace with Markdown.                        | Use Markdown bold: `**word**`                                |
| `@c word`, `@p word`                                         | Replace with Markdown.                        | Use Markdown inline code: `` `word` ``                       |
| `@f$ ... @f$`                                                | Replace with Markdown.                        | Use Markdown inline math: `$ ... $`                          |
| `@f\[ ... @f]`                                               | Replace with Markdown.                        | Use Markdown display math: `$$ ... $$`.                      |
| `@li`, `@arg`                                                | Replace with Markdown.                        | Use Markdown unordered list item: `  - `                     |
| `@code ... @endcode`, `@verbatim ... @endverbatim`           | Replace with Markdown.                        | Use Markdown display code: `` ``` ... ``` ``.                |
| `@attention`, `@bug`, `@example`, `@note`, `@todo`, `@warning`, `@remark`, `@remarks` | Replace with Markdown.                        | Use Markdown admonition: `!!! type`.                         |
| `@ref name text`                                             | Replace with Markdown.                        | Use Markdown link: `[text](#name)`                           |
| `@image format file alt`                                     | Ignored.                                      | Use Markdown image: `![alt](file)`                           |
| `@returns`, `@result`                                        | As `@return`.                                 | Use `@return`.                                               |
| `@throws` , `@exception`                                     | As `@throw`.                                  | Use `@throw`.                                                |
| `@sa`                                                        | As `@see`.                                    | Use `@see`.                                                  |
| `@brief`, `@short`                                           | Override one-sentence description.            | Make the first sentence of the documentation comment usable as a brief description. |
| `@internal`                                                  | Documentation comment and entity are hidden. | Use a normal comment, rather than a documentation comment.   |
| `@defgroup`                                                  | Ignored.                                      | Doxide handles groups differently, see below.                |
| `@file`                                                      | Ignored.                                      | Doxide does not produce documentation for files. Incorporate into the documentation for another entity, or into a custom page. |
| `@def MACRO`                                                 | Ignored.                                      | Doxide does not run the preprocessor. Add a documentation comment immediately before the `#define`. |
| `@var name`, `@fn name`, `@class name`, `@struct name`, `@union name`, `@enum name`,  `@typedef name`, `@namespace name`, `@interface name`, `@protocol name`, `@property name`. | Ignored.                                      | Add a documentation comment immediately before the relevant entity. |

