#include "doxide/Node.hpp"

void Node::add(const Node& node) {
  if (node.type == NodeType::NAMESPACE) {
    namespaces.insert({node.name, node});
  } else if (node.type == NodeType::TYPE) {
    types.insert({node.name, node});
  } else if (node.type == NodeType::VARIABLE) {
    variables.insert({node.name, node});
  } else if (node.type == NodeType::FUNCTION) {
    functions.insert({node.name, node});
  } else if (node.type == NodeType::OPERATOR) {
    operators.insert({node.name, node});
  } else {
    warn("unregonized node type, ignoring");
  }
}
