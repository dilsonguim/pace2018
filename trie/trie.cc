#include "trie.h"

string vec_printer(vector<int>& v) {
  stringstream s;
  for(auto& x : v) {
    s << " " << x;  
  }
  return s.str();
}

string edge_printer(vector<vector<int>>& v){
  stringstream s;
  for(auto& x : v) {
    s << " (" << vec_printer(x) << ")";  
  }
  return s.str();
}

static bool IsZero(vector<int>& coloring) {
  for(auto& c : coloring) {
    if(c) {
      return false;  
    }  
  }  
  return true;
}

Trie* Trie::Build(vector<int>& coloring) {
  int i = 0;
  Trie* node = this;
  while(i < coloring.size()) {
    if(node->children.size() < coloring.size()) {
      node->children.resize(coloring.size());
    }
    int c = coloring[i];
    if(node->children[c] == NULL) {
      node->known_children.push_back(c);
      node->children[c].reset(new Trie());
    }
    node = node->children[c].get();
    i++;
  }
  node->colors = coloring;
  return node;
}


Trie* Trie::Query(vector<int>& coloring) {
  int i = 0;
  Trie* node = this;
  while(i < coloring.size() && node != NULL) {
    node = (*node)[coloring[i]];
    i++;
  }
  return node;
}

Trie* Trie::FillNext(Trie* prev, int* size) {
  if(known_children.empty()) {
    (*size)++;
    prev->next = this;  
    return this;
  }
  for(auto& c : known_children) {
    prev = children[c]->FillNext(prev, size);
  }
  return prev;
}

