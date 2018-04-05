#include "brute_force.h"
#include "dsu.h"

static void Powerset(int i, int n, vector<int>& curr, vector<bool>& is_terminal,
             vector<vector<int>>& powerset) {
  if(i == n) {
    if(curr.size()) {
      powerset.push_back(curr);
    }
    return;
  }
  if(!is_terminal[i]) {
    Powerset(i+1, n, curr, is_terminal, powerset);
  }
  curr.push_back(i);
  Powerset(i+1, n, curr, is_terminal,  powerset);
  curr.pop_back();
}

Solution* Kruskal(int n, vector<int>& subset, vector<pair<int, pair<int, int>>>& edges) {
  Solution* sol = new Solution();
  DSU dsu(n);
  int i = 0, es = 0;
  for(auto& e : edges) {
    int u = e.second.first, v = e.second.second;
    if(binary_search(subset.begin(), subset.end(), u) &&
       binary_search(subset.begin(), subset.end(), v) && dsu.Find(u) != dsu.Find(v)) {
      es++;
      sol->edges.push_back({e.second.first, e.second.second, e.first});
      sol->val += e.first;
      dsu.Union(u,v);
    }
  }
  if(es < subset.size()-1) {
    delete sol;
    sol = NULL;
  }
  return sol;
}

Solution* BruteForceSolve(Bell& bell) {
  auto& graph = bell.graph;
  auto& is_terminal = bell.is_terminal;
  vector<pair<int, pair<int, int>>> edges;
  int i = 1;
  while(i < graph.size()) {
    int j = 0;
    while(j < graph[i].size()) {
      if(i < graph[i][j].first) {
        edges.push_back({graph[i][j].second, {i, graph[i][j].first}});
      }
      j++;  
    }
    i++;  
  }
  sort(edges.begin(), edges.end());
  vector<vector<int>> powerset;
  vector<int> curr;
  Powerset(1, graph.size(), curr, is_terminal, powerset);
  unique_ptr<Solution> opt;
  for(auto& s : powerset) {
    Solution* next = Kruskal(graph.size(), s, edges);
    if(next != NULL) {
      if(opt == NULL || opt->val > next->val) {
        opt.reset(next);
      }
    }
  }
  return opt.release();
}
