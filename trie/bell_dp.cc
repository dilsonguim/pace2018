#include "bell_dp.h"
#include "dsu.h"

void Bell::Solve(int bag) {
  if(tree[bag].empty()) {
    //cerr << "Solving leaf bag " << bag << ": " << vec_printer(bags[bag]) << endl;
    SolveLeaf(bag);  
  }  
  else {
    Solve(tree[bag][0]);
    if(tree[bag].size() == 1) {
      if(bags[tree[bag][0]].size() < bags[bag].size()) {
        //cerr << "Solving introduce bag " << bag << ": " << vec_printer(bags[bag]) << endl;
        SolveIntroduce(bag);  
      }
      else {
        //cerr << "Solving forget bag " << bag << ": " << vec_printer(bags[bag]) << endl;
        SolveForget(bag);  
      }
    }
    else {
      Solve(tree[bag][1]);
      //cerr << "Solving join bag " << bag << ": " << vec_printer(bags[bag]) << endl;
      SolveJoin(bag);
    }
  }
  dp[bag].get()->FillNext(dp[bag].get());
  DisclaimDP(bag);
}

void Bell::SolveLeaf(int bag) {
  int vertex = bags[bag].front();
  vector<int> coloring(2);
  coloring[1] = 1;
  dp[bag].reset(new Trie());
  dp[bag]->Build(coloring)->val = 0;
  if(!is_terminal[vertex]) {
    coloring[1] = 0;
    dp[bag]->Build(coloring)->val = 0;
  }
}

//a is bigger than b.
static int find_diff(vector<int>& a, vector<int>& b) {
  int i = 0;
  while(i < b.size()) {
    if(a[i] != b[i]) {
      break;  
    }
    i++;
  }
  return a[i];
}

static void FixIntroduceColoring(vector<int>& coloring, int phi_v) {
  coloring.push_back(0);
  int i = coloring.size()-1;
  while(i > phi_v) {
    if(coloring[i-1] >= phi_v) {
      coloring[i-1]++; 
    }
    swap(coloring[i], coloring[i-1]);
    i--;  
  }
}

static void InducedNeighbourhood(vector<pair<int, int>>& total, vector<int>& bag,
                                 vector<pair<int, int>>& v_neighbourhood, vector<int>& bag_pos) {
  int i = 0, j = 0;
  while(i < total.size() && j < bag.size()) {
    if(total[i].first == bag[j]) {
      v_neighbourhood.push_back(total[i]);
      bag_pos.push_back(j);
      i++;
      j++;
    } 
    else if(total[i].first < bag[j]) {
      i++;  
    }
    else {
      j++;
    }
  }
}

static void ColoringCost(vector<pair<int, int>>& neighbourhood, vector<int>& bag_pos,
                         vector<int>& coloring, unordered_map<int, pair<int,int>>& color_cost) {
  int i = 0;
  while(i < neighbourhood.size()) {
    int i_color = coloring[bag_pos[i]+1];
    if(i_color) {
      if(!color_cost.count(i_color)) {
        color_cost[i_color].first = neighbourhood[i].second; // edge cost
        color_cost[i_color].second = bag_pos[i]+1; // secondary index
      }
      color_cost[i_color].first = min(color_cost[i_color].first, neighbourhood[i].second);
      color_cost[i_color].second = min(color_cost[i_color].second, bag_pos[i]+1);
    }
    i++;  
  }
}

static void Powerset(Trie* trie, unordered_map<int, pair<int, int>>& color_cost,
                     unordered_map<int, pair<int, int>>::iterator it_color_cost,
                     vector<int>& base_coloring, unordered_set<int>& recoloring,
                     int min_secondary_index, long long val, long long base_val,
                     int bag) {
  if(it_color_cost == color_cost.end()) {
    vector<int> new_coloring = base_coloring;
    for(auto& color : new_coloring) {
      color = (recoloring.count(color)) ? min_secondary_index : color;
    }
    if(trie->Query(new_coloring)) {
      //cerr << "I've been here before!!!" << endl;  
    }
    else {
      //cerr << "Building new coloring!" << endl;  
    }
    Trie* node = trie->Build(new_coloring);
    //cerr << "On introduce bag " << bag << ", received " << vec_printer(base_coloring)
    //     << " with cost " << base_val << ", built " << vec_printer(new_coloring)
    //     << " with new cost " << val << " and prev cost " <<  node->val << endl; 
    node->val = min(node->val, val);
    return;
  }
  int next_color = it_color_cost->first, next_cost = it_color_cost->second.first,
      next_secondary_index = it_color_cost->second.second;

  auto it = it_color_cost;
  it++;
  Powerset(trie, color_cost, it, base_coloring, recoloring, min_secondary_index, val, base_val, bag);

  recoloring.insert(next_color);
  min_secondary_index = min(min_secondary_index, next_secondary_index);
  Powerset(trie, color_cost, it, base_coloring, recoloring, min_secondary_index,
           val + next_cost, base_val, bag);

  recoloring.erase(next_color);
}

void Bell::SolveIntroduce(int bag) {
  int child_bag = tree[bag][0];
  int v = find_diff(bags[bag], bags[child_bag]); //extra vertex in |bag|.
  int phi_v = (lower_bound(bags[bag].begin(), bags[bag].end(), v) - bags[bag].begin()) + 1;

  dp[bag].reset(new Trie());
  Trie* trie = dp[bag].get();

  vector<pair<int, int>> v_neighbourhood;
  vector<int> bag_pos;
  InducedNeighbourhood(graph[v], bags[bag], v_neighbourhood, bag_pos);

  Trie* sol = dp[child_bag].get();
  sol = sol->next;
  while(sol != NULL) {
    vector<int> coloring = sol->colors;
    //cerr << "Introduce base coloring = " << vec_printer(coloring) << endl;
    FixIntroduceColoring(coloring, phi_v);
    if(!is_terminal[v]) {
      //cerr << "Default coloring " << vec_printer(coloring) << " with val " << sol->val << endl;
      trie->Build(coloring)->val = sol->val;
    }
    coloring[phi_v] = phi_v;
    unordered_map<int, pair<int, int>> color_cost;
    ColoringCost(v_neighbourhood, bag_pos, coloring, color_cost);
    auto it_color_cost = color_cost.begin();
    unordered_set<int> recoloring;
    recoloring.insert(phi_v);
    Powerset(trie, color_cost, it_color_cost, coloring, recoloring, phi_v, sol->val, sol->val, bag);
    sol = sol->next;
  }
}

static bool FixForgetColoring(vector<int>& coloring, int phi_v) {
  int i = phi_v;
  int v_color = coloring[phi_v], m = coloring.size();
  bool other_v_colored = false;
  while(i < coloring.size()-1) {
    swap(coloring[i], coloring[i+1]);
    m = (coloring[i] == phi_v) ? min(m, i) : m;
    i++;  
  }
  coloring.pop_back();
  i = 1;
  while(i < coloring.size()) {
    other_v_colored |= (v_color && coloring[i] == v_color);
    if(coloring[i] == phi_v) {
      coloring[i] = m;  
    }
    else if(coloring[i] > phi_v) {
      coloring[i]--;  
    }
    i++;
  }
  return other_v_colored || !v_color;
}

void Bell::SolveForget(int bag) {
  int child_bag = tree[bag][0];
  int v = find_diff(bags[child_bag], bags[bag]); //extra vertex in |child_bag|.
  int phi_v = (lower_bound(bags[child_bag].begin(), bags[child_bag].end(), v)
              - bags[child_bag].begin()) + 1;

  dp[bag].reset(new Trie());
  Trie* trie = dp[bag].get();

  vector<pair<int, int>> v_neighbourhood;
  vector<int> bag_pos;
  InducedNeighbourhood(graph[v], bags[bag], v_neighbourhood, bag_pos);

  Trie* sol = dp[tree[bag][0]].get();
  sol = sol->next;
  while(sol != NULL) {
    vector<int> coloring = sol->colors;
    //cerr << "Forget base coloring = " << vec_printer(coloring) << ", base val = "
    //     << sol->val << ", forgetting phi_v = " << phi_v << endl;
    if(FixForgetColoring(coloring, phi_v)) {
      //cerr << "Fixed to coloring " << vec_printer(coloring) << endl;
      Trie* node = trie->Build(coloring);
      node->val = min(node->val, sol->val);
    }
    sol = sol->next;
  }
}

static void PaintingDFS(int root, int color, vector<vector<int>>& auxiliary_graph,
                        vector<int>& visits) {
  visits[root] = color;
  for(auto s : auxiliary_graph[root]) {
    if(!visits[s]) {
      PaintingDFS(s, color, auxiliary_graph, visits);
    }
  }
}

static bool Merge(vector<int>& l_coloring, vector<int>& r_coloring, vector<int>& j_coloring) {
  int tw = l_coloring.size();
  DSU dsu(r_coloring.size());
  vector<int> loops(tw, 0); 
  vector<vector<int>> auxiliary_graph(tw);
  int i = 1;
  while(i < l_coloring.size()) {
    int lc = l_coloring[i], rc = r_coloring[i]; 
    if(lc && rc) {
      auxiliary_graph[lc].push_back(rc);
      auxiliary_graph[rc].push_back(lc);
      loops[lc] += (lc == rc);
      if(loops[lc] > 1 || (lc != rc && dsu.Find(lc) == dsu.Find(rc))) {
        return false;
      }
      dsu.Union(lc, rc);
    }
    i++;  
  }
  i = 1;
  loops.assign(tw, 0);
  while(i < auxiliary_graph.size()) {
    if(!loops[i]) {
      PaintingDFS(i, i, auxiliary_graph, loops);
    }
    i++;
  }
  i = 1;
  j_coloring = l_coloring;
  while(i < j_coloring.size()) {
    int c = (l_coloring[i]) ? l_coloring[i] : r_coloring[i];
    j_coloring[i] = loops[c]; 
    i++;
  }
  return true;
}

void Bell::SolveJoin(int bag) {
  int left = tree[bag][0], right = tree[bag][1];  

  dp[bag].reset(new Trie());
  Trie* trie = dp[bag].get();

  Trie* l_trie = dp[left].get();
  Trie* r_trie = dp[right].get();
  Trie* sol = l_trie->next;
  while(sol != NULL) {
    Trie* other_sol = r_trie->next;
    while(other_sol != NULL) {
      vector<int> coloring;
      //cerr << "Trying to merge " << vec_printer(sol->colors) << ", to "
      //     << vec_printer(other_sol->colors) << endl;
      if(Merge(sol->colors, other_sol->colors, coloring)) {
        //cerr << "\tSuccesfully built " << vec_printer(coloring) << endl;
        trie->Build(coloring)->val = sol->val + other_sol->val;
      }
      other_sol = other_sol->next;
    }
    sol = sol->next;
  }
}

void Bell::DisclaimDP(int bag) {
  //cerr << "Disclaiming DP for bag " << bag << endl;
  Trie* sol = dp[bag]->next;
  int i = 0;
  while(sol != NULL && (i++ < 5)) {
    //cerr << "coloring = " << vec_printer(sol->colors) << ", val = " << sol->val << endl;
    sol = sol->next;
  }
}

bool IsValidRoot(vector<int>& coloring) {
  unordered_set<int> cs;
  for(int i = 1; i < coloring.size(); i++) {
    cs.insert(coloring[i]);
  }
  return cs.size() == 1;
}

long long Bell::RootValue(int bag) {
  Trie* sol = dp[bag]->next;  
  long long res = 1234567;
  while(sol != NULL) {
    res = (IsValidRoot(sol->colors)) ? min(res, sol->val) : res;
    sol = sol->next;
  }
  return res;
}
