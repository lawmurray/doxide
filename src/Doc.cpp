#include "Doc.hpp"

#include "DocTokenizer.hpp"
#include "Log.hpp"
#include "doxide.hpp"

#include <algorithm>
#include <cctype>
#include <ostream>
#include <string_view>
#include <unordered_set>
#include <utility>

Doc::Doc(const TextLineCursor &comment, const int init_indent,
    const std::string& style) :
    indent(init_indent),
    hide(false) {
  DocTokenizer tokenizer(comment);
  DocToken token = tokenizer.next();
  open = token;
  if (open.type == OPEN_BEFORE || open.type == OPEN_AFTER) {
    /* documentation comment, not just an ordinary comment */
    token = tokenizer.next();
    bool first = true;  // is this the first token in the comment?
    if (!token.type) {
      /* empty end-of-line comment, consider end of paragraph */
      if (indent && style == "pandoc") {
        docs.append("\n:::");
      }
      indent = std::max(indent - 4, 0);
    } else while (token.type) {
      if (token.type & COMMAND) {
        auto command_token = token.substr(1);
        std::string_view command = command_token.view();
        if (command == "param" ||
            command == "param[in]") {
          if (style == "mkdocs") {
            docs.append("\n:material-location-enter: `");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n:   ");
          } else if (style == "pandoc") {
            docs.append("\n`");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n:   ");
          } else {
            docs.append("\n  - `");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n\n    ");
          }
        } else if (command == "param[out]") {
          docs.append("\n");
          if (style == "mkdocs") {
            docs.append(":material-location-exit: `");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n:   ");
          } else if (style == "pandoc") {
            docs.append("`");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n:   ");
          } else {
            docs.append("\n  - `");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n\n    ");
          }
        } else if (command == "param[in,out]") {
          docs.append("\n");
          if (style == "mkdocs") {
            docs.append(":material-location-enter::material-location-exit: `");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n:   ");
          } else if (style == "pandoc") {
            docs.append("`");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n:   ");
          } else {
            docs.append("\n  - `");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n\n    ");
          }
        } else if (command == "tparam") {
          docs.append("\n");
          if (style == "mkdocs") {
            docs.append(":material-code-tags: `");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n:   ");
          } else if (style == "pandoc") {
            docs.append("`");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n:   ");
          } else {
            docs.append("\n  - `");
            docs.append(tokenizer.consume(WORD).view());
            docs.append("`\n\n    ");
          }
        } else if (command == "p") {
          docs.append("`");
          docs.append(tokenizer.consume(WORD).view());
          docs.append("`");
        } else if (command == "return" ||
            command == "returns" ||
            command == "result") {
          docs.append("\n");
          if (style == "mkdocs") {
            docs.append(":material-keyboard-return: **Return**\n:   ");
          } else if (style == "pandoc") {
            docs.append("Return\n:   ");
          } else {
            docs.append("\n  - **Return**\n");
            docs.append("\n\n    ");
          }
        } else if (command == "pre") {
          docs.append("\n");
          if (style == "mkdocs") {
            docs.append(":material-check-circle-outline: **Pre-condition**\n:   ");
          } else if (style == "pandoc") {
            docs.append("Pre-condition\n:   ");
          } else {
            docs.append("\n  - **Pre-condition**\n");
            docs.append("\n\n    ");
          }
        } else if (command == "post") {
          docs.append("\n");
          if (style == "mkdocs") {
            docs.append(":material-check-circle-outline: **Post-condition**\n:   ");
          } else if (style == "pandoc") {
            docs.append("Post-condition\n:   ");
          } else {
            docs.append("\n  - **Post-condition**\n");
            docs.append("\n\n    ");
          }
        } else if (command == "throw") {
          docs.append("\n");
          if (style == "mkdocs") {
            docs.append(":material-alert-circle-outline: **Throw**\n:   ");
          } else if (style == "pandoc") {
            docs.append("Throw\n:   ");
          } else {
            docs.append("\n  - **Throw**\n");
            docs.append("\n\n    ");
          }
        } else if (command == "see" ||
            command == "sa") {
          docs.append("\n");
          if (style == "mkdocs") {
            docs.append(":material-eye-outline: **See**\n:   ");
          } else if (style == "pandoc") {
            docs.append("See\n:   ");
          } else {
            docs.append("\n  - **See**\n");
            docs.append("\n\n    ");
          }
        } else if (command == "anchor") {
          docs.append("<a name=\"");
          docs.append(tokenizer.consume(WORD).view());
          docs.append("\"></a>");
        } else if (command == "note" ||
            command == "abstract" ||
            command == "info" ||
            command == "tip" ||
            command == "success" ||
            command == "question" ||
            command == "warning" ||
            command == "failure" ||
            command == "danger" ||
            command == "bug" ||
            command == "example" ||
            command == "quote") {
          if (style == "mkdocs") {
            docs.append("\n!!! ");
            docs.append(command);
            docs.append("\n");
          } else if (style == "pandoc") {
            docs.append("\n::: ");
            docs.append(command);
            docs.append("\n");
          } else {
            docs.append("> **");
            docs.append(capitalize(command));
            docs.append("**\n");
            docs.append("> ");
          }
          indent += 4;
        } else if (command == "ingroup") {
          ingroup = tokenizer.consume(WORD).get();

        /* legacy commands */
        } else if (command == "file" ||
            command == "internal") {
          hide = true;
        } else if (command == "e" ||
            command == "em" ||
            command == "a") {
          docs.append("*");
          docs.append(tokenizer.consume(WORD).view());
          docs.append("*");
        } else if (command == "b") {
          docs.append("**");
          docs.append(tokenizer.consume(WORD).view());
          docs.append("**");
        } else if (command == "c") {
          docs.append("`");
          docs.append(tokenizer.consume(WORD).view());
          docs.append("`");
        } else if (command == "f$") {
          docs.append("$");
        } else if (command == "f[" ||
            command == "f]") {
          docs.append("$$");
        } else if (command == "li" ||
            command == "arg") {
          docs.append("  - ");
        } else if (command == "ref") {
          auto href = tokenizer.consume(WORD);
          auto text = tokenizer.consume(WORD);
          docs.append("[");
          docs.append(text.view());
          docs.append("](#");
          docs.append(href.view());
          docs.append(")");
        } else if (command == "code" ||
            command == "endcode" ||
            command == "verbatim" ||
            command == "endverbatim") {
          docs.append("\n```");
        } else if (command == "attention") {
          if (style == "mkdocs") {
            docs.append("\n!!! warning \"Attention\"\n");
          } else if (style == "pandoc") {
            docs.append("\n::: warning\n");
          } else {
            docs.append("> **Attention**\n");
            docs.append("> ");
          }
          indent += 4;
        } else if (command == "todo") {
          if (style == "mkdocs") {
            docs.append("\n!!! example \"To-do\"\n");
          } else if (style == "pandoc") {
            docs.append("\n::: example\n");
          } else {
            docs.append("> **To-do**\n");
            docs.append("> ");
          }
          indent += 4;
        } else if (command == "remark") {
          if (style == "mkdocs") {
            docs.append("\n!!! quote \"Remark\"\n");
          } else if (style == "pandoc") {
            docs.append("\n::: quote\n");
          } else {
            docs.append("> **Remark**\n");
            docs.append("> ");
          }
          indent += 4;
        } else if (command == "def" ||
            command == "var" ||
            command == "fn" ||
            command == "class" ||
            command == "struct" ||
            command == "union" ||
            command == "enum" ||
            command == "typedef" ||
            command == "namespace" ||
            command == "interface" ||
            command == "protocol" ||
            command == "property") {
          /* ignore, including following name */
          tokenizer.consume(WORD);
        } else if (command == "@") {
          docs.append("@");
        } else if (command == "/") {
          docs.append("/");
        } else if (token.view().at(0) == '\\') {
          /* unrecognized command starting with legacy backslash, could just
           * be e.g. a LaTeX macro, output as is */
          docs.append(token.view());
        } else {
          /* keep track of warnings and don't repeat them */
          static std::unordered_set<std::string> warned;
          if (warned.insert(std::string(command)).second) {
            warn("line " << command_token.get_line_number()
              << ": unrecognized command '" << command << "'");
          }
          docs.append(token.view
            ());
        }
      } else if (token.type & PARA) {
        if (!first) {
          if (indent && style == "pandoc") {
            docs.append("\n:::");
          }
          docs.append("\n\n");
          indent = std::max(indent - 4, 0);
        }
      } else if (token.type & LINE) {
        if (!first) {
          docs.append("\n");
          add_indent(indent, style);
        }
      } else if (token.type & CLOSE) {
        if (style == "pandoc") {
          while (indent) {
            docs.append("\n:::");
            indent -= 4;
          }
        }
        indent = 0;
      } else {
        add_indent(first*indent, style);  // indent on first token
        docs.append(token.view());
      }
      token = tokenizer.next();
      first = false;
    }

    /* trim whitespace from the end and add single new line */
    while (docs.length() > 0 && std::isspace(docs.back())) {
      docs.resize(docs.length() - 1);
    }
    docs.append("\n");
  }
}

void Doc::add_indent(const int indent, const std::string& style) {
  if (style == "mkdocs") {
    docs.append(indent, ' ');
  } else if (style == "pandoc") {
    //
  } else {
    for (int i = 0; i < indent; i += 4) {
      docs.append("> ");
    }
  }
}
