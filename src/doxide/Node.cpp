#include "doxide/Node.hpp"

void Node::add(const Node& node) {
  if (node.type == NodeType::NAMESPACE) {
    /* multiple named namespace definitions are allowed, and are merged */
    if (namespaces.contains(node.name)) {
      namespaces[node.name].merge(node);
    } else {
      namespaces[node.name] = node;
    }
  } else if (node.type == NodeType::TYPE) {
    types.insert({node.name, node});
  } else if (node.type == NodeType::VARIABLE) {
    variables.insert({node.name, node});
  } else if (node.type == NodeType::FUNCTION) {
    functions.insert({node.name, node});
  } else if (node.type == NodeType::OPERATOR) {
    operators.insert({node.name, node});
  } else {
    warn("unrecognized node type, ignoring");
  }
}

void Node::merge(const Node& node) {
  docs.append(node.docs);
  namespaces.insert(node.namespaces.begin(), node.namespaces.end());
  types.insert(node.types.begin(), node.types.end());
  variables.insert(node.variables.begin(), node.variables.end());
  functions.insert(node.functions.begin(), node.functions.end());
  operators.insert(node.operators.begin(), node.operators.end());
}
