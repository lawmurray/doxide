#include "StyleParsers.hpp"

#include "Doc.hpp"
#include "DocToken.hpp"
#include "DocTokenizer.hpp"
#include "Log.hpp"
#include "Sink.hpp"
#include "TextLineCursor.hpp"
#include "Utils.hpp"
#include "DocParser.hpp"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>

using ossink_t = _sink_t<std::ostringstream, std::ostreambuf_iterator<char>>;

struct MkDocsStyle {
  ossink_t &sink;

  void end_of_paragraph() {}

  void param_in(std::string_view word) {
    sink.print(":material-location-enter: `{}`\n:   ", word);
  }

  void param_out(std::string_view word) {
    sink.print(":material-location-exit: `{}`\n:   ", word);
  }

  void param_in_out(std::string_view word) {
    sink.print(":material-location-enter::material-location-exit: `{}`\n:   ", word);
  }

  void tparam(std::string_view word) {
    sink.print(":material-code-tags: `{}`\n:   ", word);
  }

  void return_() { sink.print(":material-keyboard-return: **Return**\n:   "); }

  void pre() { sink.print(":material-check-circle-outline: **Pre-condition**\n:   "); }

  void post() { sink.print(":material-check-circle-outline: **Post-condition**\n:   "); }

  void throw_() { sink.print(":material-alert-circle-outline: **Throw**\n:   "); }

  void see() { sink.print(":material-eye-outline: **See**\n:   "); }

  void command(std::string_view command) {
    sink.print("\n!!! {}", command);
  }

  void attention() { sink.print("\n!!! warning \"Attention\"\n"); }

  void todo() { sink.print("\n!!! example \"To-do\"\n"); }

  void remark() { sink.print("\n!!! quote \"Remark\"\n"); }

  void add_indent(int indent) {
    sink.print("{}", repeater{indent});
  }
};

struct PandocStyle {
  ossink_t &sink;

  void end_of_paragraph() {
    sink.print("\n:::");
  }

  void param_in(std::string_view word) {
    sink.print("`{}`\n:   ", word);
  }

  void param_out(std::string_view word) {
    param_in(word);
  }

  void param_in_out(std::string_view word) {
    param_in(word);
  }

  void tparam(std::string_view word) {
    sink.print("**`{}`**\n:   ", word);
  }

  void return_() { sink.print("Return\n:   "); }

  void pre() { sink.print("Pre-condition\n:   "); }

  void post() { sink.print("Post-condition\n:   "); }

  void throw_() { sink.print("Throw\n:   "); }

  void see() { sink.print("See\n:   "); }

  void command(std::string_view command) {
    sink.print("\n::: {}\n", command);
  }

  void attention() { sink.print("\n::: warning\n"); }

  void todo() { sink.print("\n::: example\n"); }

  void remark() { sink.print("\n::: quote\n"); }

  void add_indent(int) {}

};

struct PlainStyle {
  ossink_t sink;

  void end_of_paragraph() {}

  void param_in(std::string_view word) {
    sink.print("  - **`{}`**\n\n    ", word);
  }

  void param_out(std::string_view word) {
    param_in(word);
  }

  void param_in_out(std::string_view word) {
    param_in(word);
  }

  void tparam(std::string_view word) {
    param_in(word);
  }

  void return_() { sink.print("\n  - **Return**\n\n\n    "); }

  void pre() { sink.print("\n  - **Pre-condition**\n\n\n    "); }

  void post() { sink.print("\n  - **Post-condition**\n\n\n    "); }

  void throw_() { sink.print("\n  - **Throw**\n\n\n    "); }

  void see() { sink.print("\n  - **See**\n\n\n    "); }

  void command(std::string_view command) {
    sink.print("> **{}**\n> ", to_upper{command});
  }

  void attention() { sink.print("> **Attention**\n> "); }

  void todo() { sink.print("> **To-do**\n> "); }

  void remark() { sink.print("> **Remark**\n> "); }

  void add_indent(int indent) {
    for (int i = 0; i < indent; i += 4) {
      sink.print("> ");
    }
  }
};


template<typename Style> class BaseParser: public DocParser {
public:

  BaseParser() {}

  Doc parse(const TextLineCursor &comment) override {
    int indent = 0;
    bool hide = false;
    DocTokenizer tokenizer(comment);
    DocToken open = tokenizer.next();
    std::ostringstream docsbuf;
    ossink_t sink(docsbuf);
    Style style(sink);
    TextLineCursor ingroup;

    if (open.type != OPEN_BEFORE and open.type != OPEN_AFTER) {
      throw std::logic_error("expected doc opening (e.g., /**, /**<, ///, ///<, etc.), but got other");
    }

    /* documentation comment, not just an ordinary comment */
    DocToken token = tokenizer.next();
    bool first = true;  // is this the first token in the comment?
    if (!token.type) {
      /* empty end-of-line comment, consider end of paragraph */
      if (indent) {
        style.end_of_paragraph();
      }
      indent = std::max(indent - 4, 0);
    } else while (token.type) {
      if (token.type & COMMAND) {
        auto command_token = token.substr(1);
        std::string_view command = command_token.view();
        if (command == "param" ||
            command == "param[in]") {
          sink.println();
          style.param_in(tokenizer.consume(WORD).view());
          indent = 4;
        } else if (command == "param[out]") {
          sink.println();
          style.param_out(tokenizer.consume(WORD).view());
        } else if (command == "param[in,out]") {
          sink.println();
          style.param_in_out(tokenizer.consume(WORD).view());
        } else if (command == "tparam") {
          sink.println();
          style.tparam(tokenizer.consume(WORD).view());
        } else if (command == "p") {
          sink.print("**`{}`**", tokenizer.consume(WORD).view());
        } else if (command == "return" ||
                    command == "returns" ||
                    command == "result") {
          sink.println();
          style.return_();
        } else if (command == "pre") {
          sink.println();
          style.pre();
        } else if (command == "post") {
          sink.println();
          style.post();
        } else if (command == "throw") {
          sink.println();
          style.throw_();
        } else if (command == "see" ||
                    command == "sa") {
          sink.println();
          style.see();
        } else if (command == "anchor") {
          sink.print("<a name=\"{}\"></a>", tokenizer.consume(WORD).view());
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
          style.command(command);
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
          sink.print("*{}*", tokenizer.consume(WORD).view());
        } else if (command == "b") {
          sink.print("**{}**", tokenizer.consume(WORD).view());
        } else if (command == "c") {
          sink.print("`{}`", tokenizer.consume(WORD).view());
        } else if (command == "f$") {
          sink.print("$", tokenizer.consume(WORD).view());
        } else if (command == "f[" ||
            command == "f]") {
          sink.print("$$", tokenizer.consume(WORD).view());
        } else if (command == "li" ||
            command == "arg") {
          sink.print("  - ", tokenizer.consume(WORD).view());
        } else if (command == "ref") {
          auto href = tokenizer.consume(WORD).view();
          auto text = tokenizer.consume(WORD).view();
          sink.print("[{}](#{})", text, href);
        } else if (command == "code" ||
            command == "endcode" ||
            command == "verbatim" ||
            command == "endverbatim") {
          sink.print("\n```");
        } else if (command == "attention") {
          style.attention();
          indent += 4;
        } else if (command == "todo") {
          style.todo();
          indent += 4;
        } else if (command == "remark") {
          style.remark();
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
          sink.print("@");
        } else if (command == "/") {
          sink.print("/");
        } else if (auto t = token.view(); t.at(0) == '\\') {
          /* unrecognized command starting with legacy backslash, could just
            * be e.g. a LaTeX macro, output as is */
          sink.print("{}", t);
        } else {
          /* keep track of warnings and don't repeat them */
          static std::unordered_set<std::string> warned;
          if (warned.insert(std::string(command)).second) {
            warn("line " << command_token.get_line_number()
              << ": unrecognized command '" << command << "'");
          }
          sink.print("{}", token.view());
        }
      } else if (token.type & PARA) {
        if (!first) {
          if (indent) {
            style.end_of_paragraph();
          }
          sink.print("\n\n");
          indent = std::max(indent - 4, 0);
        }
      } else if (token.type & LINE) {
        if (!first) {
          sink.print("\n");
          style.add_indent(indent);
        }
      } else if (token.type & CLOSE) {
        while (indent) {
          style.end_of_paragraph();
          indent -= 4;
        }
        indent = 0;
      } else {
        style.add_indent(first * indent);  // indent on first token
        sink.print("{}", token.view());
      }
      token = tokenizer.next();
      first = false;
    }

    std::string docs = std::move(docsbuf).str();

    /* trim whitespace from the end and add single new line */
    unsigned to_trim = 0;
    for(auto rit = docs.rbegin(); rit != docs.rend() and std::isspace(*rit); ++rit) {
      to_trim++;
    }
    if (to_trim > 0) {
      docs[docs.size() - to_trim] = '\n';
      docs.resize(docs.length() - to_trim + 1);
    } else {
      docs.append("\n");
    }

    return Doc(std::move(docs), std::move(ingroup), hide);
  }
};

std::unique_ptr<DocParser> make_doc_parser(StyleOption style) {
  switch (style) {
    case StyleOption::MKDOCS: {
      return std::unique_ptr<DocParser>(new BaseParser<MkDocsStyle>());
    }
    case StyleOption::PANDOCS: {
      return std::unique_ptr<DocParser>(new BaseParser<PandocStyle>());
    }
    case StyleOption::PLAIN: {
      return std::unique_ptr<DocParser>(new BaseParser<PlainStyle>());
    }
  }
}
