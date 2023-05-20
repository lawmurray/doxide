#include "doxide/Node.hpp"

Node::Node() :
   type(NodeType::NONE),
   hide(false) {
  //
}

void Node::add(const Node& node) {
  /* namespaces should not be added this way, instead retrieve with ns() */
  assert(node.type != NodeType::NAMESPACE);

  if (!node.hide) {
    if (!node.ingroup.empty() && node.ingroup != name) {
      group(node.ingroup).add(node);
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
    } else {
      warn("unrecognized node type, ignoring");
    }
  }
}

Node& Node::ns(const std::string& name) {
  auto iter = namespaces.find(name);
  if (iter != namespaces.end()) {
    return iter->second;
  } else {
    Node node;
    node.type = NodeType::NAMESPACE;
    node.name = name;
    namespaces.insert({name, node});
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
    groups.insert({name, node});
    return group(name);
  }
}
