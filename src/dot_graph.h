//Copyright 2007 baj
//baj@mail.ustc.edu.cn

#ifndef _DOT_GRAPH_H_
#define _DOT_GRAPH_H_

#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

namespace dot {

struct Label {
  std::string label;

  Label() {

  }

  Label(const std::string &str) {
    label = str;
  }

  friend std::ostream &operator<<(std::ostream &os, const Label &l) {
    if (l.label.length()) {
      return os << "[label=\"" << l.label << "\"]";
    } else {
      return os;
    }
  }
};

struct Color {
  std::string color;

  Color() {

  }

  Color(const std::string &str) {
    color = str;
  }

  friend std::ostream &operator<<(std::ostream &os, const Color &c) {
    if (c.color.length()) {
      return os << "[color=\"" << c.color << "\"]";
    } else {
      return os;
    }
  }
};

struct Edge {
  Color color;
  Label label;
};

class Node {
public:
  int id;
  Label label;
  Color color;

  std::unordered_map<int, Edge> neighbours;

  Node() {
  }

  Node(int i, Label l) {
    id = i;
    label = l;
  }

  void setColor(const std::string &str) {
    color = Color(str);
  }

  void addNeighbor(const int n, const std::string &edge_color, const std::string &edge_label);

  friend std::ostream &operator<<(std::ostream &os, const Node &n) {
    os << n.id << " " << n.label;
    if (n.color.color.length()) {
      os << n.color;
    }
    return os;
  }
};

class Graph {
  std::vector<Node> nodes;

public:
  const std::vector<Node> &getNodes() const;

private:
  std::unordered_map<std::string, int> id_map;

public:
  Graph();

  void addNode(const std::string &name, const std::string &color = "");

  void addNode(const int n, const std::string &color = "");

  void
  addEdge(const std::string &a, const std::string &b, const std::string &color = "", const std::string &label = "");

  void addEdge(const int a, const int b, const std::string &color = "", const std::string &label = "");

  friend std::ostream &operator<<(std::ostream &os, const Graph &g);

  void dump(const std::string &filename);
};

} //namspace dot_graph

#endif
