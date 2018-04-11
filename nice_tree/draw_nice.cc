#include "../trie/trie.h"
#include "draw_nice.h"

static bool has_terminal(vector<int>& bag, vector<bool>& is_terminal) {
  bool res = false;
  for(auto& x : bag) {
    res = res | is_terminal[x];
  }  
  return res;
}

static void InnerDraw(int u, set<int>& parents, vector<vector<int>>& tree,
                      vector<vector<int>>& bags, vector<bool>& is_terminal,
                      ostream& out) {
  out << "\t" << u << " [label=\"" << u << ": " << vec_printer(bags[u]) << "\"";
  if (has_terminal(bags[u], is_terminal)) {
    out << ", color=red";
  }
  out << "]" << ";" << endl;
  for (auto& c : tree[u]) {
    out << "\t" << u << " -- " << c << ";" << endl;
    if (!parents.count(c)) {
      parents.insert(c);
      InnerDraw(c, parents, tree, bags, is_terminal, out);
    }
  }
}

void Draw(vector<vector<int>>& tree, vector<vector<int>>& bags,
          vector<bool>& is_terminal, ostream& out) {
  out << "graph {" << endl;
  set<int> parents;
  int i = 1;
  while (i < tree.size()) {
    if (!parents.count(i)) {
      parents.insert(i);
      InnerDraw(i, parents, tree, bags, is_terminal, out);
    }
    i++;
  }

  out << "}";
}

static void InnerDrawGraph(int u, set<int>& parents,
                           vector<vector<pair<int, int>>>& graph,
                           vector<bool>& is_terminal, ostream& out) {
  parents.insert(u);
  out << "\t" << u;
  if (is_terminal[u]) {
    out << " [color=red";
    out << "]";
  }
  out << ";" << endl;
  for (auto& c : graph[u]) {
    if (u < c.first) {
      out << "\t" << u << " -- " << c.first << " [label=\"" << c.second
          << "\"];" << endl;
    }
    if (!parents.count(c.first)) {
      InnerDrawGraph(c.first, parents, graph, is_terminal, out);
    }
  }
}

void DrawGraph(vector<vector<pair<int, int>>>& graph, vector<bool>& is_terminal,
               ostream& out) {
  out << "graph {" << endl;
  set<int> parents;
  int i = 1;
  while (i < graph.size()) {
    if (!parents.count(i)) {
      InnerDrawGraph(i, parents, graph, is_terminal, out);
    }
    i++;
  }
  out << "}";
}
