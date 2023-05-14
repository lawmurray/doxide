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
    token = consume(NAMESPACE|DOC);
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
  token = consume(WORD);
  node.name = token.str();
  token = consume(BRACE|SEMICOLON);
  node.decl = std::string_view(first.first, token.first);

  /* members */
  if (token.type & BRACE) {
    do {
      token = consume(NAMESPACE|DOC|BRACE_CLOSE);
      if (token.type & NAMESPACE) {
        node.add(parseNamespace(token));
      } else if (token.type & DOC) {
        node.add(parseDocs(token));
      }
    } while (token.type && !(token.type & BRACE_CLOSE));
  }
  return node;
}

Node Parser::parseDocs(const Token& first) {
  static const auto useful =
      NAMESPACE|TYPE|TILDE|EQUALS|BRACE|SEMICOLON|PAREN|OPERATOR;

  Node node;
  Token from = consume(~WHITESPACE);
  Token scan = from.type & useful ? from : consume(useful);

  if (scan.type & NAMESPACE) {
    /* namespace */
    node = parseNamespace(scan);
  } else if (scan.type & TYPE) {
    /* type */
    node.type = NodeType::TYPE;

    /* name */
    scan = consume(WORD);
    node.name = scan.str();

    /* signature */
    scan = consume(BRACE|SEMICOLON);
    node.decl = std::string_view(from.first, scan.first);

    /* members */
    if (scan.type & BRACE) {
      do {
        scan = consume(DOC|BRACE_CLOSE);
        if (scan.type & DOC) {
          node.add(parseDocs(scan));
        }
      } while (scan.type && !(scan.type & BRACE_CLOSE));
    }
  } else if (scan.type & (EQUALS|BRACE|SEMICOLON)) {
    /* variable, e.g. int x; int x = 0; int x{0}; */
    node.type = NodeType::VARIABLE;
    node.name = word.str();
    node.decl = std::string_view(from.first, scan.first);

    if (scan.type & (EQUALS|BRACE)) {
      /* skip to the end of the statement */
      scan = consume(SEMICOLON);
    }
  } else if (scan.type & TILDE) {
    /* destructor */
    node.type = NodeType::FUNCTION;

    /* name */
    scan = consume(WORD);
    node.name = "~";
    node.name += scan.str();

    /* signature */
    scan = consume(SEMICOLON|BRACE|COLON);
    node.decl = std::string_view(from.first, scan.first);

    if (scan.type & BRACE) {
      /* skip over the body */
      scan = consume(BRACE_CLOSE);
    }
  } else if (scan.type & PAREN) {
    /* function */
    node.type = NodeType::FUNCTION;
    node.name = word.str();

    /* signature */
    scan = consume(PAREN_CLOSE);
    scan = consume(SEMICOLON|BRACE|COLON);
    node.decl = std::string_view(from.first, scan.first);

    if (scan.type & COLON) {
      /* skip the initializer list */
      scan = consume(BRACE);
    }
    if (scan.type & BRACE) {
      /* skip the body */
      scan = consume(BRACE_CLOSE);
    }
  } else if (scan.type & OPERATOR) {
    /* operator */
    node.type = NodeType::OPERATOR;

    /* name */
    Token name = scan;
    scan = consume(PAREN);
    node.name = std::string_view(name.first, scan.first);

    /* signature */
    scan = consume(PAREN_CLOSE);
    scan = consume(SEMICOLON|BRACE);
    node.decl = std::string_view(from.first, scan.first);

    if (scan.type & BRACE) {
      /* skip the body */
      scan = consume(BRACE_CLOSE);
    }
  }

  node.docs = first.str();
  return node;
}

Token Parser::consume(const uint64_t stop) {
  Token token = tokenizer.next();
  if (token.type & WORD) {
    word = token;
  }
  while (token.type && !(token.type & stop)) {
    if (token.type & (BRACE|BRACKET|PAREN)) {
      /* consume to a matching close, which is one left shift away */
      token = consume(token.type << 1);
    } else if (token.type & ANGLE) {
      /* ambiguity with operators < > <= >=, consume to any close */
      token = consume(BRACE_CLOSE|BRACKET_CLOSE|PAREN_CLOSE|ANGLE_CLOSE);
      if (!(token.type & ANGLE_CLOSE)) {
        /* unmatched close, treat as an operator not a delimiter */
        return token;
      }
    }
    token = tokenizer.next();
    if (token.type & WORD) {
      word = token;
    }
  }
  return token;
}
