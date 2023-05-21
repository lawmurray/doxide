#include "yaml/YAMLParser.hpp"

YAMLParser::YAMLParser() {
  std::filesystem::path path;
  if (std::filesystem::exists("doxide.yaml")) {
    path = "doxide.yaml";
  } else if (std::filesystem::exists("doxide.yml")) {
    path = "doxide.yml";
  } else if (std::filesystem::exists("doxide.json")) {
    path = "doxide.json";
  } else {
    std::cerr << "no doxide configuration file, use 'doxide init' to get set up." << std::endl;
    exit(EXIT_FAILURE);
  }
  file = fopen(path.string().c_str(), "r");
  if (!file) {
    std::cerr << "could not open file " << path.string() << std::endl;
    exit(EXIT_FAILURE);
  }  
}

YAMLParser::~YAMLParser() {
  fclose(file);
}

YAMLParser::map_type YAMLParser::parse() {
  yaml_parser_initialize(&parser);
  yaml_parser_set_input_file(&parser, file);
  int done = 0;
  while (!done) {
    if (!yaml_parser_parse(&parser, &event)) {
      std::cerr << "syntax error in build configuration file." << std::endl;
      exit(EXIT_FAILURE);
    }
    if (event.type == YAML_SEQUENCE_START_EVENT) {
      parseSequence();
    } else if (event.type == YAML_MAPPING_START_EVENT) {
      parseMapping();
    } else {
      done = event.type == YAML_STREAM_END_EVENT;
      yaml_event_delete(&event);
    }
  }
  yaml_parser_delete(&parser);
  return contents;
}

void YAMLParser::parseMapping() {
  yaml_event_delete(&event);
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
      std::string key(data, length);
      yaml_event_delete(&event);

      if (keys.empty()) {
        keys.push(key);
      } else {
        keys.push(keys.top() + "." + key);
      }
      
      /* value */
      if (!yaml_parser_parse(&parser, &event)) {
        std::cerr << "syntax error in configuration file." << std::endl;
        exit(EXIT_FAILURE);
      }
      if (event.type == YAML_SCALAR_EVENT) {
        parseScalar();
      } else if (event.type == YAML_SEQUENCE_START_EVENT) {
        parseSequence();
      } else if (event.type == YAML_MAPPING_START_EVENT) {
        parseMapping();
      } else {
        yaml_event_delete(&event);
      }

      keys.pop();
    } else {
      done = event.type == YAML_MAPPING_END_EVENT;
      yaml_event_delete(&event);
    }
  }
}

void YAMLParser::parseSequence() {
  yaml_event_delete(&event);
  int done = 0;
  while (!done) {
    if (!yaml_parser_parse(&parser, &event)) {
      std::cerr << "syntax error in configuration file." << std::endl;
      exit(EXIT_FAILURE);
    }
    if (event.type == YAML_SCALAR_EVENT) {
      parseScalar();
    } else if (event.type == YAML_SEQUENCE_START_EVENT) {
      parseSequence();
    } else if (event.type == YAML_MAPPING_START_EVENT) {
      parseMapping();
    } else {
      done = event.type == YAML_SEQUENCE_END_EVENT;
      yaml_event_delete(&event);
    }
  }
}

void YAMLParser::parseScalar() {
  auto data = (char*)event.data.scalar.value;
  auto length = event.data.scalar.length;
  std::string value(data, length);
  contents[keys.top()].push_back(value);
  yaml_event_delete(&event);
}
