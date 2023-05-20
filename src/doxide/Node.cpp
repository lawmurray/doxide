#include "doxide/Node.hpp"

Node::Node() :
   type(NodeType::NONE),
   hide(false) {
  //
}

void Node::add(const Node& node) {
  if (!node.hide) {
    if (!node.ingroup.empty() && node.ingroup != name) {
      group(node.ingroup).add(node);
    } else if (node.type == NodeType::NAMESPACE) {
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
    } else if (node.type == NodeType::MACRO) {
      macros.insert({node.name, node});
    } else if (node.type == NodeType::GROUP) {
      groups.insert({node.name, node});
    } else if (node.type == NodeType::FILE) {
      warn("@file is not supported");
    } else {
      warn("unrecognized node type, ignoring");
    }
  }
}

void Node::merge(const Node& node) {
  docs.append(node.docs);
  namespaces.insert(node.namespaces.begin(), node.namespaces.end());
  types.insert(node.types.begin(), node.types.end());
  variables.insert(node.variables.begin(), node.variables.end());
  functions.insert(node.functions.begin(), node.functions.end());
  operators.insert(node.operators.begin(), node.operators.end());
  groups.insert(node.groups.begin(), node.groups.end());
}

Node& Node::ns(const std::string& name) {
  auto iter = namespaces.find(name);
  if (iter != namespaces.end()) {
    return iter->second;
  } else {
    Node node;
    node.type = NodeType::NAMESPACE;
    node.name = name;
    add(node);
    return ns(name);
  }
}

Node& Node::group(const std::string& name) {
  auto iter = groups.find(name);
  if (iter != groups.end()) {
    return iter->second;
  } else {
    Node node;
    node.type = NodeType::GROUP;
    node.name = name;
    add(node);
    return group(name);
  }
}
