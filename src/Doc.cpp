#include "Doc.hpp"
#include "DocTokenizer.hpp"

Doc::Doc(const std::string_view comment) : hide(false) {
  DocTokenizer tokenizer(comment);
  DocToken token = tokenizer.next();
  open = token;
  if (open.type == OPEN_BEFORE || open.type == OPEN_AFTER) {
    /* documentation comment, not just an ordinary comment */
    token = tokenizer.next();
    int indent = 0;
    bool first = true;  // is this the first token in the comment?
    if (!token.type) {
      /* empty end-of-line comment, consider end of paragraph */
      indent = std::max(indent - 4, 0);
    } else while (token.type) {
      if (token.type & COMMAND) {
        std::string_view command = token.substr(1);

        /* non-legacy commands */
        if (command == "param" ||
            command == "param[in]") {
          docs.append("\n:material-location-enter: `");
          docs.append(tokenizer.consume(WORD).str());
          docs.append("`\n:   ");
          indent = 4;
        } else if (command == "param[out]") {
          docs.append("\n:material-location-exit: `");
          docs.append(tokenizer.consume(WORD).str());
          docs.append("`\n:   ");
          indent = 4;
        } else if (command == "param[in,out]") {
          docs.append("\n:material-location-enter::material-location-exit: `");
          docs.append(tokenizer.consume(WORD).str());
          docs.append("`\n:   ");
          indent = 4;
        } else if (command == "tparam") {
          docs.append("\n:material-code-tags: `");
          docs.append(tokenizer.consume(WORD).str());
          docs.append("`\n:   ");
          indent = 4;
        } else if (command == "p") {
          docs.append("`");
          docs.append(tokenizer.consume(WORD).str());
          docs.append("`");
        } else if (command == "return") {
          docs.append("\n:material-keyboard-return: **Return**\n:   ");
        } else if (command == "pre") {
          docs.append("\n:material-check-circle-outline: **Pre-condition**\n:   ");
        } else if (command == "post") {
          docs.append("\n:material-check-circle-outline: **Post-condition**\n:   ");
        } else if (command == "throw") {
          docs.append("\n:material-alert-circle-outline: **Throw**\n:   ");
        } else if (command == "see") {
          docs.append("\n:material-eye-outline: **See**\n:   ");
        } else if (command == "anchor") {
          docs.append("<a name=\"");
          docs.append(tokenizer.consume(WORD).str());
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
          docs.append("\n!!! ");
          docs.append(command);
          docs.append("\n");
          indent += 4;
          docs.append(indent, ' ');
        } else if (command == "ingroup") {
          ingroup = tokenizer.consume(WORD).str();

        /* legacy commands */
        } else if (command == "returns" ||
            command == "result") {
          docs.append("\n:material-location-exit: **Return**\n:   ");
        } else if (command == "sa") {
          docs.append("\n:material-eye-outline: **See**\n:   ");
        } else if (command == "file" ||
            command == "internal") {
          hide = true;
        } else if (command == "e" ||
            command == "em" ||
            command == "a") {
          docs.append("*");
          docs.append(tokenizer.consume(WORD).str());
          docs.append("*");
        } else if (command == "b") {
          docs.append("**");
          docs.append(tokenizer.consume(WORD).str());
          docs.append("**");
        } else if (command == "c") {
          docs.append("`");
          docs.append(tokenizer.consume(WORD).str());
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
          docs.append(text.str());
          docs.append("](#");
          docs.append(href.str());
          docs.append(")");
        } else if (command == "code" ||
            command == "endcode" ||
            command == "verbatim" ||
            command == "endverbatim") {
          docs.append("\n```");
        } else if (command == "attention") {
          docs.append("\n!!! warning \"Attention\"\n");
          indent = 4;
          docs.append(indent, ' ');
        } else if (command == "todo") {
          docs.append("\n!!! example \"To-do\"\n");
          indent = 4;
          docs.append(indent, ' ');
        } else if (command == "remark") {
          docs.append("\n!!! quote \"Remark\"\n");
          indent = 4;
          docs.append(indent, ' ');
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
        } else if (token.str().at(0) == '\\') {
          /* unrecognized command starting with legacy backslash, could just
          * be e.g. a LaTeX macro, output as is */
          docs.append(token.str());
        } else {
          /* keep track of warnings and don't repeat them */
          static std::unordered_set<std::string> warned;
          if (warned.insert(std::string(command)).second) {
            warn("unrecognized command: " << command);
          }
          docs.append(token.str());
        }
      } else if (token.type & PARA) {
        if (!first) {
          docs.append("\n\n");
          indent = std::max(indent - 4, 0);
        }
      } else if (token.type & LINE) {
        if (!first) {
          docs.append("\n");
          docs.append(indent, ' ');
        }
      } else if (token.type & CLOSE) {
        //
      } else {
        docs.append(first*indent, ' ');  // indent on first token
        docs.append(token.str());
      }
      token = tokenizer.next();
      first = false;
    }

    /* trim whitespace from the end */
    while (docs.length() > 0 && std::isspace(docs.back())) {
      docs.resize(docs.length() - 1);
    }

    /* append end of paragraph; this ensures that multiple doc comments that
     * occur for an entity are treated as separate paragraphs */
    if (!docs.empty()) {
      docs.append("\n\n");
    }
  }
}
