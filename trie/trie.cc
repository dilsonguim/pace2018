#include "trie.h"

string vec_printer(vector<int>& v) {
  stringstream s;
  for(auto& x : v) {
    s << " " << x;  
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

void Trie::Spawn(int tw) {
  vector<int> coloring(tw+2);
  do {
    int t = tw+1;
    while(t) {
      if(coloring[t] == t) {
        coloring[t] = 0;
      }
      else {
        int i = coloring[t]+1;
        while(i < t && coloring[i] <= coloring[t]) {
          i++;  
        }
        coloring[t] = i;
      }
      t--;
    }
    Build(coloring);  
  } while(!IsZero(coloring));
}


Trie* Trie::Build(vector<int>& coloring) {
  int i = 1;
  Trie* node = this;
  while(i < coloring.size()) {
    if(!node->children.count(coloring[i])) {
      node->children[coloring[i]].reset(new Trie());
    }
    node = node->children[coloring[i]].get();
    i++;
  }
  node->colors = coloring;
  return node;
}


Trie* Trie::Query(vector<int>& coloring) {
  int i = 1;
  Trie* node = this;
  while(i < coloring.size() && node != NULL) {
    node = (*node)[coloring[i]];
    i++;
  }
  return node;
}

Trie* Trie::FillNext(Trie* prev) {
  if(children.empty()) {
    prev->next = this;  
    return this;
  }
  for(auto& c : children) {
    prev = c.second->FillNext(prev);
  }
  return prev;
}

