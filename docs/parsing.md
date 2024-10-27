# Parsing Considerations

Doxide may report parsing errors in C++ source files, even if they are building successfully with a C++ compiler. This can occur for one of two reasons:

1. The parser used, [Tree-sitter](https://tree-sitter.github.io), is different to that of a compiler, and is designed more for language tools such as syntax highlighters. It may fail to parse code that is accepted by a compiler, or even accept code that is rejected by a compiler.

2. Doxide does not run a preprocessor before parsing (it is unclear that this would even be desirable). Consequently, it may report parsing errors in situations where C++ syntax is only valid after preprocessing, due to the use of preprocessor macros.

To demonstrate the latter case, consider the following macro definition:
```cpp
#define NO_INLINE __attribute__((noinline))
```
This could be used in a function declaration to direct the compiler not to inline the function:
```cpp
NO_INLINE int f(int x, int y);
```
When the preprocessor is run, the function declaration expands to the valid C++ syntax:
```cpp
__attribute__((noinline)) int f(int x, int y);
```
But prior to running the preprocessor, this is not valid C++ syntax. For example, a parser may consider `NO_INLINE` the return type and the following `int` an error.

When a parse error is encountered, Doxide issues a warning, and applies some error recovery logic. If that error recovery logic fails, it simply skips the problematic tokens and continues the parse. In many cases such issues are inconsequential to the final documentation and can be safely ignored, e.g. a parse error in the body of a function will produce a warning, but have no affect on its documentation.

Doxide has both manual and automated means for resolving these issues.

## Specifying preprocessor symbols

The `defines` section of the [configuration file](configuring.md) may be used to set preprocessor symbols and their replacements---a sort of rudimentary substitute for a preprocessor. This also allows the symbols to be replaced with different values in the documentation than in the code. For example, the `NO_INLINE` example above could be resolved with the following in the configuration file:
```yaml
defines:
  NO_INLINE: __attribute__((noinline))
```
or, if we prefer not to include such detail in the documentation, it could be erased with:
```yaml
defines:
  NO_INLINE: ""
```

## Autocorrection

Because parse errors are usually the result of preprocessor use, Doxide has specific error recovery logic to attempt to autocorrect them. 

1. When an error is encountered on a node in the parse tree, it steps back through the siblings of that node one by one.

2. If it finds a sibling that looks like a preprocessor symbol, it erases that sibling and retries the parse.

3. Otherwise, it ignores the parse error and continues.

For the purposes of step 2, following the usual convention for preprocessor symbols, a sibling looks like a preprocessor symbol if it has at least three characters consisting of uppercase letters, underscores, and digits, but starting with an uppercase letter or underscore.
