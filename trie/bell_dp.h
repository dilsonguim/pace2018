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
#include <unordered_set>
#include <memory>
#include "trie.h"


struct Bell {

  void Solve(int bag);

  void SolveLeaf(int bag);
  void SolveIntroduce(int bag);
  void SolveForget(int bag);
  void SolveJoin(int bag);
  void DisclaimDP(int bag);
  Trie* RootSolution(int bag);
    
  vector<vector<pair<int, int>>> graph;
  vector<bool> is_terminal;
  vector<vector<int>> tree;
  vector<vector<int>> bags;
  vector<unique_ptr<Trie>> dp;
};

