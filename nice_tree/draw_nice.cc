#include "draw_nice.h"
#include "../trie/trie.h"

static void InnerDraw(int u, set<int>& parents, vector<vector<int>>& tree,
                      vector<vector<int>>& bags, vector<bool>& is_terminal, ostream& out) {
  parents.insert(u);
  out << "\t" << u << " [label=\"" << u << ": " << vec_printer(bags[u]) << "\"";
  if(is_terminal[u]) {
    out << ", color=red";  
  }
  out << "]" << ";" <<  endl;
  for(auto& c : tree[u]) {
    if(u < c) {
      out << "\t" << u << " -- " << c << ";" << endl;
    }
    if(!parents.count(c)) {
      InnerDraw(c, parents, tree, bags, is_terminal, out);
    }  
  }
} 

void Draw(int root, vector<vector<int>>& tree, vector<vector<int>>& bags,
          vector<bool>& is_terminal, ostream& out) {
  out << "graph {" << endl;
  set<int> parents;
  InnerDraw(root, parents, tree, bags, is_terminal, out);
  out << "}";
}

static void InnerDrawGraph(int u, set<int>& parents, vector<vector<pair<int, int>>>& graph,
                      vector<vector<int>>& bags, vector<bool>& is_terminal, ostream& out) {
  parents.insert(u);
  out << "\t" << u;
  if(is_terminal[u]) {
    out << " [color=red";  
    out << "]" << ";";
  }
  cout << endl;
  for(auto& c : graph[u]) {
    if(u < c.first && graph[c.first].size()) {
      out << "\t" << u << " -- " << c.first << " [label=\"" << c.second << "\"];" << endl;
    }
    if(!parents.count(c.first) && graph[c.first].size()) {
      InnerDrawGraph(c.first, parents, graph, bags, is_terminal, out);
    }  
  }
} 
void DrawGraph(vector<vector<pair<int, int>>>& graph, vector<bool>& is_terminal, ostream& out) {
  out << "graph {" << endl;
  set<int> parents;
  vector<vector<int>> bags(graph.size());
  InnerDrawGraph(1, parents, graph, bags, is_terminal, out);
  out << "}";
}


