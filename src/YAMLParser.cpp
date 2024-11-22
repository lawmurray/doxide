#include "YAMLParser.hpp"

YAMLParser::YAMLParser(const std::filesystem::path& filename) :
    filename(filename) {
  yaml_parser_initialize(&parser);
}

YAMLParser::~YAMLParser() {
  yaml_parser_delete(&parser);
}

YAMLNode YAMLParser::parse() {
  YAMLNode root;
  FILE* file = fopen(filename.c_str(), "r");
  if (!file) {
    throw std::runtime_error("could not read file " + filename);
  }

  yaml_parser_set_input_file(&parser, file);
  bool done = false;
  int docs = 0;
  try {
    while (!done) {
      if (!yaml_parser_parse(&parser, &event)) {
        throw std::runtime_error("YAML syntax error in file " + filename);
      }
      if (event.type == YAML_SEQUENCE_START_EVENT) {
        parseSequence(root);
      } else if (event.type == YAML_MAPPING_START_EVENT) {
        parseMapping(root);
      } else if (event.type == YAML_DOCUMENT_END_EVENT ||
          event.type == YAML_STREAM_END_EVENT) {
        done = true;
        yaml_event_delete(&event);
      } else {
        yaml_event_delete(&event);
      }
    }
  } catch (const std::runtime_error& e) {
    /* close file before propagating exception */
    fclose(file);
    throw e;
  }
  fclose(file);

  return root;
}

void YAMLParser::parseMapping(YAMLNode& node) {
  yaml_event_delete(&event);
  node.setMapping();
  int done = 0;
  while (!done) {
    /* read one name/value pair on each iteration */
    if (!yaml_parser_parse(&parser, &event)) {
      throw std::runtime_error("YAML syntax error in file " + filename);
    }
    if (event.type == YAML_SCALAR_EVENT) {
      /* key */
      auto data = (char*)event.data.scalar.value;
      auto length = event.data.scalar.length;
      auto key = std::string{data, length};
      yaml_event_delete(&event);

      /* value */
      if (!yaml_parser_parse(&parser, &event)) {
        throw std::runtime_error("YAML syntax error in file " + filename);
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
      throw std::runtime_error("YAML syntax error in file " + filename);
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
