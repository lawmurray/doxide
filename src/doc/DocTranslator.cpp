#include "doc/DocTranslator.hpp"
#include "doc/DocTokenizer.hpp"

void DocTranslator::translate(const std::string_view& comment,
    Entity& entity) {
  std::string result;
  int indent = 0;
  DocTokenizer tokenizer(comment);
  DocToken token = tokenizer.next();
  if (token.type & DOC) {  // otherwise not a documentation comment
    token = tokenizer.next();
    while (token.type) {
      if (token.type & DOC_COMMAND) {
        /* non-legacy commands */
        if (token.substr(1) == "param" ||
            token.substr(1) == "param[in]") {
          result.append(":material-location-enter: **Parameter** `");
          result.append(tokenizer.consume(WORD).str());
          result.append("`\n:   ");
        } else if (token.substr(1) == "param[out]") {
          result.append(":material-location-exit: **Parameter** `");
          result.append(tokenizer.consume(WORD).str());
          result.append("`\n:   ");
        } else if (token.substr(1) == "param[in,out]") {
          result.append(":material-location-enter::material-location-exit: **Parameter** `");
          result.append(tokenizer.consume(WORD).str());
          result.append("`\n:   ");
        } else if (token.substr(1) == "tparam") {
          result.append(":material-code-tags: **Template parameter** `");
          result.append(tokenizer.consume(WORD).str());
          result.append("`\n:   ");
        } else if (token.substr(1) == "p") {
          result.append("`");
          result.append(tokenizer.consume(WORD).str());
          result.append("`");
        } else if (token.substr(1) == "return") {
          result.append(":material-location-exit: **Return**\n:   ");
        } else if (token.substr(1) == "pre") {
          result.append(":material-check-circle-outline: **Pre-condition**\n:   ");
        } else if (token.substr(1) == "post") {
          result.append(":material-check-circle-outline: **Post-condition**\n:   ");
        } else if (token.substr(1) == "throw") {
          result.append(":material-alert-circle-outline: **Throw**\n:   ");
        } else if (token.substr(1) == "see") {
          result.append(":material-eye-outline: **See**\n:   ");
        } else if (token.substr(1) == "anchor") {
          result.append("<a name=\"");
          result.append(tokenizer.consume(WORD).str());
          result.append("\"></a>");
        } else if (token.substr(1) == "note" ||
            token.substr(1) == "abstract" ||
            token.substr(1) == "info" ||
            token.substr(1) == "tip" ||
            token.substr(1) == "success" ||
            token.substr(1) == "question" ||
            token.substr(1) == "warning" ||
            token.substr(1) == "failure" ||
            token.substr(1) == "danger" ||
            token.substr(1) == "bug" ||
            token.substr(1) == "example" ||
            token.substr(1) == "quote") {
          result.append("!!! ");
          result.append(token.substr(1));
          indent += 4;
          result.append(indent, ' ');
        } else if (token.substr(1) == "ingroup") {
          entity.ingroup = tokenizer.consume(WORD).str();

        /* legacy commands */
        } else if (token.substr(1) == "returns" ||
            token.substr(1) == "result") {
          result.append(":material-location-exit: **Return**\n:   ");
        } else if (token.substr(1) == "sa") {
          result.append(":material-eye-outline: **See**\n:   ");
        } else if (token.substr(1) == "file") {
          entity.type = EntityType::FILE;
        } else if (token.substr(1) == "internal") {
          entity.hide = true;
        } else if (token.substr(1) == "brief" ||
            token.substr(1) == "short") {
          auto first = tokenizer.consume(~WHITESPACE);
          auto last = tokenizer.consume(SENTENCE);
          entity.brief.append(first.first, last.last);
        } else if (token.substr(1) == "e" ||
            token.substr(1) == "em" ||
            token.substr(1) == "a") {
          result.append("*");
          result.append(tokenizer.consume(WORD).str());
          result.append("*");
        } else if (token.substr(1) == "b") {
          result.append("**");
          result.append(tokenizer.consume(WORD).str());
          result.append("**");
        } else if (token.substr(1) == "c") {
          result.append("`");
          result.append(tokenizer.consume(WORD).str());
          result.append("`");
        } else if (token.substr(1) == "f$") {
          result.append("$");
        } else if (token.substr(1) == "f[" ||
            token.substr(1) == "f]") {
          result.append("$$");
        } else if (token.substr(1) == "li" ||
            token.substr(1) == "arg") {
          result.append("  - ");
        } else if (token.substr(1) == "ref") {
          auto href = tokenizer.consume(WORD);
          auto text = tokenizer.consume(WORD);
          result.append("[");
          result.append(text.str());
          result.append("](#");
          result.append(href.str());
          result.append(")");
        } else if (token.substr(1) == "code" ||
            token.substr(1) == "endcode" ||
            token.substr(1) == "verbatim" ||
            token.substr(1) == "endverbatim") {
          result.append("```");
        } else if (token.substr(1) == "attention") {
          result.append("!!! warning \"Attention\"\n");
          indent += 4;
          result.append(indent, ' ');
        } else if (token.substr(1) == "todo") {
          result.append("!!! example \"To-do\"\n");
          indent += 4;
          result.append(indent, ' ');
        } else if (token.substr(1) == "remark") {
          result.append("!!! quote \"Remark\"\n");
          indent += 4;
          result.append(indent, ' ');

        /* unrecognized commands */
        } else {
          warn("unrecognized command: " << token.str());
          result.append(token.str());
        }
      } else if (token.type & DOC_ESCAPE) {
        result.append(token.substr(1));
      } else if (token.type & DOC_PARA) {
        result.append("\n\n");
        indent = std::max(indent - 4, 0);
      } else if (token.type & DOC_LINE) {
        result.append("\n");
        result.append(indent, ' ');
      } else if (token.type & DOC_CLOSE) {
        //
      } else {
        result.append(token.str());
      }
      token = tokenizer.next();
    }
  }
}
