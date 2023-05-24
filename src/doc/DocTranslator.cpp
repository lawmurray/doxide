#include "doc/DocTranslator.hpp"
#include "doc/DocTokenizer.hpp"

void DocTranslator::translate(const std::string_view& comment,
    Entity& entity) {
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
          entity.docs.append(":material-location-enter: **Parameter** `");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("`\n:   ");
        } else if (token.substr(1) == "param[out]") {
          entity.docs.append(":material-location-exit: **Parameter** `");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("`\n:   ");
        } else if (token.substr(1) == "param[in,out]") {
          entity.docs.append(":material-location-enter::material-location-exit: **Parameter** `");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("`\n:   ");
        } else if (token.substr(1) == "tparam") {
          entity.docs.append(":material-code-tags: **Template parameter** `");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("`\n:   ");
        } else if (token.substr(1) == "p") {
          entity.docs.append("`");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("`");
        } else if (token.substr(1) == "return") {
          entity.docs.append(":material-location-exit: **Return**\n:   ");
        } else if (token.substr(1) == "pre") {
          entity.docs.append(":material-check-circle-outline: **Pre-condition**\n:   ");
        } else if (token.substr(1) == "post") {
          entity.docs.append(":material-check-circle-outline: **Post-condition**\n:   ");
        } else if (token.substr(1) == "throw") {
          entity.docs.append(":material-alert-circle-outline: **Throw**\n:   ");
        } else if (token.substr(1) == "see") {
          entity.docs.append(":material-eye-outline: **See**\n:   ");
        } else if (token.substr(1) == "anchor") {
          entity.docs.append("<a name=\"");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("\"></a>");
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
          entity.docs.append("!!! ");
          entity.docs.append(token.substr(1));
          indent += 4;
          entity.docs.append(indent, ' ');
        } else if (token.substr(1) == "group") {
          Entity group;
          group.type = EntityType::GROUP;
          group.name = tokenizer.consume(WORD).str();
          group.docs = tokenizer.consume(DOC_PARA).str();
          entity.add(group);
        } else if (token.substr(1) == "ingroup") {
          entity.ingroup = tokenizer.consume(WORD).str();

        /* legacy commands */
        } else if (token.substr(1) == "returns" ||
            token.substr(1) == "result") {
          entity.docs.append(":material-location-exit: **Return**\n:   ");
        } else if (token.substr(1) == "sa") {
          entity.docs.append(":material-eye-outline: **See**\n:   ");
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
          entity.docs.append("*");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("*");
        } else if (token.substr(1) == "b") {
          entity.docs.append("**");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("**");
        } else if (token.substr(1) == "c") {
          entity.docs.append("`");
          entity.docs.append(tokenizer.consume(WORD).str());
          entity.docs.append("`");
        } else if (token.substr(1) == "f$") {
          entity.docs.append("$");
        } else if (token.substr(1) == "f[" ||
            token.substr(1) == "f]") {
          entity.docs.append("$$");
        } else if (token.substr(1) == "li" ||
            token.substr(1) == "arg") {
          entity.docs.append("  - ");
        } else if (token.substr(1) == "ref") {
          auto href = tokenizer.consume(WORD);
          auto text = tokenizer.consume(WORD);
          entity.docs.append("[");
          entity.docs.append(text.str());
          entity.docs.append("](#");
          entity.docs.append(href.str());
          entity.docs.append(")");
        } else if (token.substr(1) == "code" ||
            token.substr(1) == "endcode" ||
            token.substr(1) == "verbatim" ||
            token.substr(1) == "endverbatim") {
          entity.docs.append("```");
        } else if (token.substr(1) == "attention") {
          entity.docs.append("!!! warning \"Attention\"\n");
          indent += 4;
          entity.docs.append(indent, ' ');
        } else if (token.substr(1) == "todo") {
          entity.docs.append("!!! example \"To-do\"\n");
          indent += 4;
          entity.docs.append(indent, ' ');
        } else if (token.substr(1) == "remark") {
          entity.docs.append("!!! quote \"Remark\"\n");
          indent += 4;
          entity.docs.append(indent, ' ');

        /* unrecognized commands */
        } else {
          warn("unrecognized command: " << token.str());
          entity.docs.append(token.str());
        }
      } else if (token.type & DOC_ESCAPE) {
        entity.docs.append(token.substr(1));
      } else if (token.type & DOC_PARA) {
        entity.docs.append("\n\n");
        indent = std::max(indent - 4, 0);
      } else if (token.type & DOC_LINE) {
        entity.docs.append("\n");
        entity.docs.append(indent, ' ');
      } else if (token.type & DOC_CLOSE) {
        //
      } else {
        entity.docs.append(token.str());
      }
      token = tokenizer.next();
    }
  }
}
