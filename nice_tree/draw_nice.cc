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

void DrawGraph(vector<vector<pair<int, int>>>& graph, vector<bool>& is_terminal, ostream& out) {
  vector<vector<int>> tree(graph.size());
  vector<vector<int>> bags(graph.size());
  for(int i = 1; i < graph.size(); i++) {
    bags[i].push_back(i);
    for(auto& x : graph[i]) {
      tree[i].push_back(x.first);  
    }
  }
  Draw(1, tree, bags, is_terminal, out);
}


