#include "doxide/Parser.hpp"

void Parser::parse(const std::string& file) {
  tokenizer.load(file);
  parseGlobal();
}

const Node& Parser::root() const {
  return global;
}

void Parser::parseGlobal() {
  Node child;
  Token token;
  do {
    token = consume(~(NAMESPACE|DOC));
    if (token.type & NAMESPACE) {
      global.add(parseNamespace(token));
    } else if (token.type & DOC) {
      global.add(parseDocs(token));
    }
  } while (token.type);
}

Node Parser::parseNamespace(const Token& first) {
  Node node;
  Token token;

  node.type = NodeType::NAMESPACE;
  token = consume(WHITESPACE);
  node.name = token.str();
  token = consume(~(BRACE|SEMICOLON));
  node.decl = std::string_view(first.first, token.last);

  /* members */
  if (token.type & BRACE) {
    do {
      token = consume(~(NAMESPACE|DOC|BRACE_CLOSE));
      if (token.type & NAMESPACE) {
        node.add(parseNamespace(token));
      } else if (token.type & DOC) {
        node.add(parseDocs(token));
      }
    } while (token.type && !(token.type & BRACE_CLOSE));
  }
  return node;
}

Node Parser::parseType(const Token& first) {
  Node node;
  Token token;

  node.type = NodeType::TYPE;
  token = consume(WHITESPACE);
  if (token.type & WORD) {
    node.name = token.str();
  }
  token = consume(~(BRACE|SEMICOLON));
  node.decl = std::string_view(first.first, token.first);

  /* members */
  if (token.type & BRACE) {
    do {
      token = consume(~(DOC|BRACE_CLOSE));
      if (token.type & DOC) {
        node.add(parseDocs(token));
      }
    } while (token.type && !(token.type & BRACE_CLOSE));
  }

  return node;
}

Node Parser::parseDocs(const Token& first) {
  Node node;
  Token token, last;

  token = consume(~COMMENT_CLOSE, false);
  std::string_view docs = std::string_view(first.first, token.last);
  token = consume(WHITESPACE);
  
  if (token.type & NAMESPACE) {
    node = parseNamespace(token);
  } else if (token.type & CLASS) {
    node = parseType(token);
  } else if (token.type & TEMPLATE) {
    /* might be a type, function, or operator (template) */
    bool done = false;
    while (!done) {
      last = consume(~(WORD|PAREN|OPERATOR|CLASS));
      if (last.type & WORD) {
        /* name of whatever comes next, may be overwritten on subsequent
         * iterations to ensure that the last word becomes the name */
        node.name = last.str();
      } else if (last.type & PAREN) {
        /* function */
        node.type = NodeType::FUNCTION;

        /* finish reading in the parameters */
        last = consume(~PAREN_CLOSE);

        /* read to the end of the signature (e.g. trailing const on member
         * functions) or start of the initializer list (for a constructor) */
        last = consume(~(SEMICOLON|BRACE|COLON));
        node.decl = std::string_view(token.first, last.first);

        /* if that was the start of an initializer list, skip ahead to the
         * opening brace */
        if (last.type & COLON) {
          last = consume(~BRACE);
        }

        /* skip over the function body, if it exists */
        if (last.type & BRACE) {
          last = consume(~BRACE_CLOSE);
        }
        done = true;
      } else if (last.type & OPERATOR) {
        /* operator */
        node.type = NodeType::OPERATOR;

        /* skip to the opening parenthesis */
        last = consume(~PAREN);
        node.name = std::string_view(token.first, last.first);

        /* finish reading in the parameters */
        last = consume(~(SEMICOLON|BRACE));
        node.decl = std::string_view(token.first, last.first);

        /* skip over the operator body, if it exists */
        if (last.type & BRACE) {
          last = consume(~BRACE_CLOSE);
        }
        done = true;
      } else if (last.type & CLASS) {
        /* type */
        node.type = NodeType::TYPE;

        /* signature */
        last = consume(WHITESPACE);
        if (last.type & WORD) {
          node.name = last.str();
        }
        last = consume(~(BRACE|SEMICOLON));
        node.decl = std::string_view(first.first, last.first);

        /* members */
        if (last.type & BRACE) {
          do {
            last = consume(~(DOC|BRACE_CLOSE));
            if (last.type & DOC) {
              node.add(parseDocs(last));
            }
          } while (last.type && !(last.type & BRACE_CLOSE));
        }
        done = true;
      }
    }
  } else {
    /* might be a variable, function, or operator */
    bool done = false;
    while (!done) {
      last = consume(~(WORD|EQUALS|BRACE|SEMICOLON|PAREN|OPERATOR));
      if (last.type & WORD) {
        /* name of whatever comes next, may be overwritten on subsequent
         * iterations to ensure that the last word becomes the name */
        node.name = last.str();
      } else if (last.type & (EQUALS|BRACE|SEMICOLON)) {
        /* variable, e.g. int x; int x = 0; int x{0}; */
        node.type = NodeType::VARIABLE;

        /* signature */
        node.decl = std::string_view(token.first, last.first);

        /* in the case an initializer, skip over it to the end of the
         * statement */
        if (last.type & (EQUALS|BRACE)) {
          last = consume(~SEMICOLON);
        }
        done = true;
      } else if (last.type & PAREN) {
        /* function */
        node.type = NodeType::FUNCTION;

        /* finish reading in the parameters */
        last = consume(~PAREN_CLOSE);

        /* read to the end of the signature (e.g. trailing const on member
         * functions) or start of the initializer list (for a constructor) */
        last = consume(~(SEMICOLON|BRACE|COLON));
        node.decl = std::string_view(token.first, last.first);

        /* if that was the start of an initializer list, skip ahead to the
         * opening brace */
        if (last.type & COLON) {
          last = consume(~BRACE);
        }

        /* skip over the function body, if it exists */
        if (last.type & BRACE) {
          last = consume(~BRACE_CLOSE);
        }
        done = true;
      } else if (last.type & OPERATOR) {
        /* operator */
        node.type = NodeType::OPERATOR;

        /* skip to the opening parenthesis */
        last = consume(~PAREN);
        node.name = std::string_view(token.first, last.first);

        /* finish reading in the parameters */
        last = consume(~(SEMICOLON|BRACE));
        node.decl = std::string_view(token.first, last.first);

        /* skip over the operator body, if it exists */
        if (last.type & BRACE) {
          last = consume(~BRACE_CLOSE);
        }
        done = true;
      }
    }
  }

  node.docs = docs;
  return node;
}

Token Parser::consume(const uint64_t valid, const bool delimiters) {
  Token token = tokenizer.next();
  while (token.type & valid) {
    if (token.type & (BRACE|BRACKET|PAREN) && delimiters) {
      /* for all of BRACE, BRACKET and PAREN, the closing counterpart in
       * TokenType is a left shift away */
      token = consume(~(token.type << 1), true);
    } else if (token.type & (DOC|COMMENT)) {
      token = consume(~COMMENT_CLOSE, false);
    }
    token = tokenizer.next();
  }
  return token;
}
