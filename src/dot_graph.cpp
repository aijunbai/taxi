#include "dot_graph.h"

#include <cstring>
#include <sstream>
#include <fstream>

using namespace std;

namespace dot {

void Node::addNeighbor(const int n, const std::string &edge_color, const std::string &edge_label) {
  Edge &edge = neighbours[n];

  edge.color = Color(edge_color);
  edge.label = Label(edge_label);
}

Graph::Graph() {
  Node dummy;
  nodes.push_back(dummy);
}

void Graph::addNode(const int n, const std::string &color) {
  std::stringstream ss;
  ss << n;
  addNode(ss.str(), color);
}

void Graph::addNode(const std::string &name, const std::string &color) {
  if (id_map.count(name)) {
    nodes[id_map[name]].setColor(color);
    return;
  }

  int id = nodes.size();

  Node node = Node(id, Label(name));
  node.setColor(color);

  nodes.push_back(node);
  id_map[name] = id;
}

void Graph::addEdge(const int a, const int b, const std::string &color, const std::string &label) {
  std::stringstream ssa, ssb;
  ssa << a;
  ssa << b;

  addEdge(ssa.str(), ssa.str(), color, label);
}

void Graph::dump(const string &filename) {
  ofstream fout(filename);
  fout << *this << endl;
  fout.close();
}

void Graph::addEdge(const std::string &a, const std::string &b, const std::string &color, const std::string &label) {
  if (!id_map.count(a)) {
    addNode(a);
  }
  if (!id_map.count(b)) {
    addNode(b);
  }

  int id_a = id_map[a];
  int id_b = id_map[b];

  nodes[id_a].addNeighbor(id_b, color, label);
}

std::ostream &operator<<(std::ostream &os, const Graph &g) {
  os << "Digraph \"\" {\nratio=fill\n";
  os << "node [style=filled];" << std::endl;

  for (unsigned int i = 1; i < g.nodes.size(); ++i) {
    os << "\t";
    os << g.nodes[i];
    os << ";" << std::endl;
  }

  for (unsigned int i = 1; i < g.nodes.size(); ++i) {
    for (auto it = g.nodes[i].neighbours.begin();
         it != g.nodes[i].neighbours.end(); ++it) {
      os << "\t" << i << " -> " << it->first << " ";
      os << it->second.color;
      os << it->second.label;
      os << ";" << std::endl;
    }
  }

  os << "}" << std::endl;

  return os;
}

const std::vector<Node> &Graph::getNodes() const {
  return nodes;
}

} //namespace dot
