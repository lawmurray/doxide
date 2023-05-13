#include "doxide/Parser.hpp"

void Parser::parse(const std::string& file) {
  tokenizer.load(file);
  parseGlobal();
}

void Parser::parseGlobal() {
  Node child;
  Token token;
  do {
    token = consume(~(NAMESPACE|CLASS|DOC));
    if (token.type & NAMESPACE) {
      global.add(parseNamespace(token));
    } else if (token.type & (CLASS)) {
      global.add(parseClass(token));
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
      token = consume(~(NAMESPACE|CLASS|DOC|BRACE_CLOSE));
      if (token.type & NAMESPACE) {
        node.add(parseNamespace(token));
      } else if (token.type & CLASS) {
        node.add(parseClass(token));
      } else if (token.type & DOC) {
        node.add(parseDocs(token));
      }
    } while (token.type && !(token.type & BRACE_CLOSE));
  }
  return node;
}

Node Parser::parseClass(const Token& first) {
  Node node;
  Token token;

  node.type = NodeType::TYPE;
  token = consume(WHITESPACE);
  node.name = token.str();
  token = consume(~(BRACE|SEMICOLON));
  node.decl = std::string_view(first.first, token.last);

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
  Token token;

  token = consume(~DOC_CLOSE, false);
  std::string_view docs = std::string_view(first.first, token.last);
  token = consume(WHITESPACE);
  if (token.type & NAMESPACE) {
    node = parseNamespace(token);
  } else if (token.type & CLASS) {
    node = parseClass(token);
  } else if (token.type & DOC) {
    node = parseDocs(token);
  } else {
    /* might be a variable, function, or operator */
    bool done = false;
    while (!done) {
      token = consume(~(WORD|EQUALS|BRACE|SEMICOLON|PAREN));
      if (token.type & WORD) {
        /* name of whatever comes next, may be overwritten on subsequent
         * iterations to ensure that the last word becomes the name */
        node.name = token.str();
      } else if (token.type & (EQUALS|BRACE|SEMICOLON)) {
        /* variable, e.g.
         *   int x;
         *   int x = 0;
         *   int x{0};
         */
        node.type = NodeType::VARIABLE;
        node.decl = std::string_view(first.first, token.last);

        /* in the case an initializer, skip over it to the end of the
         * statement */
        if (token.type & (EQUALS|BRACE)) {
          token = consume(~SEMICOLON);
        }
        done = true;
      } else if (token.type & PAREN) {
        /* function; finish reading in the parameters */
        token = consume(~(SEMICOLON|BRACE));
        node.type = NodeType::FUNCTION;
        node.decl = std::string_view(first.first, token.last);

        /* skip over the function body, if it exists */
        if (token.type & BRACE) {
          token = consume(~BRACE_CLOSE);
        }
        done = true;
      } else if (token.type & OPERATOR) {
        /* operator; skip to the opening parenthesis */
        Token name = token;
        token = consume(~PAREN);
        node.name = std::string_view(name.first, token.last);

        /* finish reading in the parameters */
        token = consume(~(SEMICOLON|BRACE));
        node.type = NodeType::OPERATOR;
        node.decl = std::string_view(first.first, token.last);

        /* skip over the operator body, if it exists */
        if (token.type & BRACE) {
          token = consume(~BRACE_CLOSE);
        }
        done = true;
      }
    }
  }

  node.docs = docs;
  return node;
}

Token Parser::consume(const uint64_t valid, const bool flat) {
  Token token;
  do {
    token = tokenizer.next();
    if (!flat && token.type & DELIMITER) {
      token = consume(~(token.type << 1), flat);
      // ^ left shift gives matching closing delimiter
    }
  } while (token.type & valid);
  return token;
}
