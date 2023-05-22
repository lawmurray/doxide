#include "yaml/YAMLParser.hpp"

YAMLParser::YAMLParser() {
  yaml_parser_initialize(&parser);
}

YAMLParser::~YAMLParser() {
  yaml_parser_delete(&parser);
}

void YAMLParser::parse(const std::string_view& contents) {
  yaml_parser_set_input_string(&parser, (const unsigned char*)contents.data(),
      contents.size());
  int done = 0;
  while (!done) {
    if (!yaml_parser_parse(&parser, &event)) {
      std::cerr << "syntax error in configuration file." << std::endl;
      exit(EXIT_FAILURE);
    }
    if (event.type == YAML_SEQUENCE_START_EVENT) {
      parseSequence(global);
    } else if (event.type == YAML_MAPPING_START_EVENT) {
      parseMapping(global);
    } else {
      done = event.type == YAML_STREAM_END_EVENT;
      yaml_event_delete(&event);
    }
  }
}

void YAMLParser::parseMapping(YAMLNode& node) {
  yaml_event_delete(&event);
  node.setMapping();
  int done = 0;
  while (!done) {
    /* read one name/value pair on each iteration */
    if (!yaml_parser_parse(&parser, &event)) {
      std::cerr << "syntax error in configuration file." << std::endl;
      exit(EXIT_FAILURE);
    }
    if (event.type == YAML_SCALAR_EVENT) {
      /* key */
      auto data = (char*)event.data.scalar.value;
      auto length = event.data.scalar.length;
      auto key = std::string{data, length};
      yaml_event_delete(&event);

      /* value */
      if (!yaml_parser_parse(&parser, &event)) {
        std::cerr << "syntax error in configuration file." << std::endl;
        exit(EXIT_FAILURE);
      }
      if (event.type == YAML_SCALAR_EVENT) {
        parseValue(node.insert(key));
      } else if (event.type == YAML_SEQUENCE_START_EVENT) {
        parseSequence(node.insert(key));
      } else if (event.type == YAML_MAPPING_START_EVENT) {
        parseMapping(node.insert(key));
      } else {
        yaml_event_delete(&event);
      }
    } else {
      done = event.type == YAML_MAPPING_END_EVENT;
      yaml_event_delete(&event);
    }
  }
}

void YAMLParser::parseSequence(YAMLNode& node) {
  yaml_event_delete(&event);
  node.setSequence();
  int done = 0;
  while (!done) {
    if (!yaml_parser_parse(&parser, &event)) {
      std::cerr << "syntax error in configuration file." << std::endl;
      exit(EXIT_FAILURE);
    }
    if (event.type == YAML_SCALAR_EVENT) {
      parseValue(node.push());
    } else if (event.type == YAML_SEQUENCE_START_EVENT) {
      parseSequence(node.push());
    } else if (event.type == YAML_MAPPING_START_EVENT) {
      parseMapping(node.push());
    } else {
      done = event.type == YAML_SEQUENCE_END_EVENT;
      yaml_event_delete(&event);
    }
  }
}

void YAMLParser::parseValue(YAMLNode& node) {
  auto data = (char*)event.data.scalar.value;
  auto length = event.data.scalar.length;
  node.set(std::string(data, length));
  yaml_event_delete(&event);
}
