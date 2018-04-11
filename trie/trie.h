#ifndef _H_TRIE_H_
#define _H_TRIE_H_

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <list>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <cmath>
#include <unordered_map>
#include <memory>
#include <limits>

using namespace std;

struct Trie {
  Trie(long long v) {
    next = NULL;
    val = v;
  }

  Trie() : Trie(std::numeric_limits<long long>::max()) {};

  unordered_map<int, unique_ptr<Trie>> children;
  
  void Spawn(int tw);

  //|coloring| at pos=0 is useless!
  Trie* Query(vector<int>& coloring);

  //|coloring| at pos=0 is useless!
  Trie* Build(vector<int>& coloring);

  Trie* operator[](int i) {
    return children.count(i) ? children[i].get() : NULL;
  }

  Trie* FillNext(Trie* prev);

  Trie* next;
  vector<int> colors;
  vector<vector<int>> edges;
  long long val;
};

string vec_printer(vector<int>& v);

string edge_printer(vector<vector<int>>& v);

#endif
