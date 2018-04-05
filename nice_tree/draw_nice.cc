#include "draw_nice.h"
#include "../trie/trie.h"

static void InnerDraw(int u, int parent, vector<vector<int>>& tree,
                      vector<vector<int>>& bags, ostream& out) {
  out << "\t" << u << " [label=\"" << vec_printer(bags[u]) << "\"]" << ";" <<  endl;
  for(auto& c : tree[u]) {
    if(c != parent) {
      out << "\t" << u << " -- " << c << ";" << endl;
      InnerDraw(c, u, tree, bags, out);
    }  
  }
} 

void Draw(int root, vector<vector<int>>& tree, vector<vector<int>>& bags, ostream& out) {
  out << "graph {" << endl;
  InnerDraw(root, -1, tree, bags, out);
  out << "}";

}
