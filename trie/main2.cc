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
#include "bell_dp.h"
#include "brute_force.h"

int main() {
  Bell dp;
  int n = 0, m = 0, t = 0;
  // #nodes #edges #terminals.
  cin >> n >> m >> t;
  dp.graph.resize(n+1);
  dp.is_terminal.assign(n+1, false);
  int i = 0;
  while(i < m) {
    int u = 0, v = 0, w = 0;
    cin >> u >> v >> w;
    dp.graph[u].push_back(make_pair(v,w));
    dp.graph[v].push_back(make_pair(u,w));
    sort(dp.graph[u].begin(), dp.graph[u].end());
    sort(dp.graph[v].begin(), dp.graph[v].end());
    i++;
  }
  i = 0;
  // |t| terminals.
  while(i < t) {
    int v = 0;
    cin >> v;
    dp.is_terminal[v] = 1;
    i++;
  }
  // tree decomposition, only children listed.
  int r = 0, n1 = n;
  // #bags root_id treewidth
  cin >> n >> r >> t;
  dp.tree.resize(n+1);
  dp.bags.resize(n+1);
  i = 0;
  while(i < n-1) {
    int u = 0, v = 0;
    cin >> u >> v;
    dp.tree[u].push_back(v);
    i++;
  }
  // bag description
  i = 0;
  while(i < n) {
    int j = 0;
    cin >> j >> m;
    while(m) {
      int u = 0;
      cin >> u;
      dp.bags[j].push_back(u);
      m--;  
    }
    sort(dp.bags[j].begin(), dp.bags[j].end());
    i++;  
  }
  vector<int> shuf(n1+1);
  for(i = 0; i < n1+1; i++) {
    shuf[i] = i;  
  }
  unordered_set<int> sols;
  int perms = 0;
  auto itb = shuf.begin(), itc = shuf.end();
  itb++;
  do {
    ++perms;
    cerr << perms << " = " << vec_printer(shuf) << endl;
    Bell dp_real;
    dp_real.graph.resize(n1+1);
    dp_real.is_terminal.resize(n1+1);
    dp_real.tree = dp.tree;
    dp_real.bags = dp.bags;
    for(i = 1; i < dp.graph.size(); i++) {
      for(auto& x : dp.graph[i]) {
        dp_real.graph[shuf[i]].push_back(make_pair(shuf[x.first], x.second));
      }
    }
    for(i = 1; i < dp_real.bags.size(); i++) {
      for(auto& x : dp_real.bags[i]) {
        x = shuf[x];
      }
      sort(dp_real.bags[i].begin(), dp_real.bags[i].end());
    }
    for(i = 1; i < dp_real.is_terminal.size(); i++) {
      sort(dp_real.graph[i].begin(), dp_real.graph[i].end());
      dp_real.is_terminal[shuf[i]] = dp.is_terminal[i];
    }
    for(i = 1; i < dp_real.is_terminal.size(); i++) {
      sort(dp_real.graph[i].begin(), dp_real.graph[i].end());
      dp_real.is_terminal[shuf[i]] = dp.is_terminal[i];
    }
    dp_real.dp.resize(n+1);
    dp_real.Solve(r);
    Trie* sol = dp_real.RootSolution(r);
    sols.insert(sol->val);
    cerr << "Dynamic programming" << endl;
    cerr << sol->val << endl;
    for(auto& e : sol->edges) {
      sort(e.begin(), e.end());
      cerr << e[0] << ", " << e[1] << endl;      
    }
    cerr << "Brute force" << endl;
    unique_ptr<Solution> bsol(BruteForceSolve(dp_real));
    cerr << bsol->val << endl;
    for(auto& e : bsol->edges) {
      cerr << e[0] << ", " << e[1] << ", " << e[2] << endl;      
    }
  } while(false); //while(next_permutation(itb, itc));
  if(sols.size() > 1) {
    cerr << "FUCK!!!!!" << endl;  
    abort();
  }
}
