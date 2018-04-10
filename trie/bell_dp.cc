#include "bell_dp.h"
#include "dsu.h"

bool Bell::Solve(int bag) {
  if(tree[bag].empty()) {
    //cerr << endl << "Solving leaf bag " << bag << ": " << vec_printer(bags[bag]) << endl;
    SolveLeaf(bag);  
  }  
  else {
    if(!Solve(tree[bag][0])) {
      return false;  
    }
    if(tree[bag].size() == 1) {
      if(bags[tree[bag][0]].size() < bags[bag].size()) {
        //cerr << endl << "Solving introduce bag " << bag << ": " << vec_printer(bags[bag]) << endl;
        SolveIntroduce(bag);  
      }
      else {
        //cerr << endl << "Solving forget bag " << bag << ": " << vec_printer(bags[bag]) << endl;
        SolveForget(bag);  
      }
    }
    else { 
      if(!Solve(tree[bag][1])) {
        return false;  
      }
      //cerr << endl << "Solving join bag " << bag << ": " << vec_printer(bags[bag]) << endl;
      SolveJoin(bag);
    }
  }
  //cerr << "solved bag: " << bag << " " << vec_printer(bags[bag]) << endl;
  if(dp[bag]->children.size()) {
    dp[bag]->FillNext(dp[bag].get());
    return true;
    //DisclaimDP(bag);
  }
  return false;  
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

struct ColorCost {
  ColorCost(int v, int e, int s) {
    vertex = v;
    edge_cost = e;
    secondary_index = s;
  }

  ColorCost() : ColorCost(-1, 123456789, 123456789) {}

  int vertex;
  int edge_cost;
  int secondary_index;

  bool operator<(const ColorCost& c) {
    return (edge_cost != c.edge_cost) ? edge_cost < c.edge_cost
                                      : secondary_index < c.secondary_index;  
  }

  string str() {
    stringstream s;
    s << "(" << vertex << ", " << edge_cost << "|" << secondary_index << ")";
    return s.str();
  }
};

static void InducedNeighbourhood(vector<pair<int, int>>& total, vector<int>& bag,
                                 vector<ColorCost>& v_neighbourhood) {
  int i = 0, j = 0;
  while(i < total.size() && j < bag.size()) {
    if(total[i].first == bag[j]) {
      // bag[0] is not useless!
      v_neighbourhood.push_back({total[i].first, total[i].second, j+1});
      //cerr << "pushed back " << v_neighbourhood.back().str() << endl;
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

static void ColoringCost(vector<ColorCost>& neighbourhood,
                         vector<int>& coloring, unordered_map<int, ColorCost>& color_cost) {
  int i = 0;
  while(i < neighbourhood.size()) {
    int i_color = neighbourhood[i].secondary_index;
    if(i_color && coloring[i_color] == i_color) {
      auto& c_cost = color_cost[i_color];
      if(neighbourhood[i] < c_cost) {
        c_cost.vertex = neighbourhood[i].vertex;
        c_cost.edge_cost = neighbourhood[i].edge_cost;
      }
      c_cost.secondary_index = min(c_cost.secondary_index,
                                   neighbourhood[i].secondary_index);
      //cerr << "for color " << i_color << ", min_secondary_index = "
      //     << c_cost.secondary_index << endl;
    }
    i++;  
  }
}

static void Powerset(Trie* trie, unordered_map<int, ColorCost>& color_cost,
                     unordered_map<int, ColorCost>::iterator it_color_cost,
                     Trie* base_solution, unordered_set<int>& recoloring,
                     int min_secondary_index, long long val, long long base_val,
                     int bag, int v, vector<vector<int>>& edges) {
  if(it_color_cost == color_cost.end()) {
    vector<int> new_coloring = base_solution->colors;
    //cerr << "Recoloring:";
    for(auto& color : recoloring) {
      //cerr << " " << color << " -> " << min_secondary_index;
    }
    //cerr << endl;
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
    //cerr << "On introduce bag " << bag << ", received " << vec_printer(base_solution->colors)
    //     << " with cost " << base_val << ", built " << vec_printer(new_coloring)
    //     << " with new cost " << val << " and prev cost " <<  node->val << endl; 
    if(node->val > val) {
      node->val = val;
      node->edges = base_solution->edges;
      for(auto& e : edges) {
        node->edges.push_back(e);
      }  
    }
    return;
  }
  auto cost = it_color_cost->second;
  int next_color = it_color_cost->first, next_cost = cost.edge_cost,
      next_secondary_index = cost.secondary_index;

  auto it = it_color_cost;
  it++;
  Powerset(trie, color_cost, it, base_solution, recoloring, min_secondary_index,
           val, base_val, bag, v, edges);

  recoloring.insert(next_color);
  edges.push_back({v, cost.vertex});
  min_secondary_index = min(min_secondary_index, next_secondary_index);
  Powerset(trie, color_cost, it, base_solution, recoloring, min_secondary_index,
           val + next_cost, base_val, bag, v, edges);

  edges.pop_back();
  recoloring.erase(next_color);
}

void Bell::SolveIntroduce(int bag) {
  int child_bag = tree[bag][0];
  int v = find_diff(bags[bag], bags[child_bag]); //extra vertex in |bag|.
  int phi_v = (lower_bound(bags[bag].begin(), bags[bag].end(), v) - bags[bag].begin()) + 1;
  //cerr << "introduced " << v << " phi_v = " << phi_v << endl;

  dp[bag].reset(new Trie());
  Trie* trie = dp[bag].get();

  vector<ColorCost> v_neighbourhood;
  InducedNeighbourhood(graph[v], bags[bag], v_neighbourhood);

  Trie* sol = dp[child_bag].get();
  sol = sol->next;
  while(sol != NULL) {
    vector<int> coloring = sol->colors;
    //cerr << "Introduce base coloring = " << vec_printer(coloring) << endl;
    FixIntroduceColoring(coloring, phi_v);
    if(!is_terminal[v]) {
      //cerr << "Default coloring " << vec_printer(coloring) << " with val " << sol->val << endl;
      trie->Build(coloring)->val = sol->val;
      trie->Query(coloring)->edges = sol->edges;
    }
    coloring[phi_v] = phi_v;
    unordered_map<int, ColorCost> color_cost;
    ColoringCost(v_neighbourhood, coloring, color_cost);
    auto it_color_cost = color_cost.begin();
    unordered_set<int> recoloring;
    recoloring.insert(phi_v);
    vector<int> prev_coloring = sol->colors;
    sol->colors = coloring;
    vector<vector<int>> edges;
    Powerset(trie, color_cost, it_color_cost, sol, recoloring, phi_v, sol->val,
             sol->val, bag, v, edges);
    sol->colors = prev_coloring;
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

  Trie* sol = dp[tree[bag][0]].get();
  sol = sol->next;
  while(sol != NULL) {
    vector<int> coloring = sol->colors;
    //cerr << "Forget base coloring = " << vec_printer(coloring) << ", base val = "
    //     << sol->val << ", forgetting phi_v = " << phi_v << endl;
    if(FixForgetColoring(coloring, phi_v)) {
      //cerr << "Fixed to coloring " << vec_printer(coloring) << endl;
      Trie* node = trie->Build(coloring);
      if(node->val > sol->val) {
        node->val = sol->val;
        node->edges = sol->edges;
      }
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
        Trie* node = trie->Build(coloring);
        node->val = sol->val + other_sol->val;
        node->edges = sol->edges;
        for(auto& e : other_sol->edges) {
          node->edges.push_back(e);
        }
      }
      other_sol = other_sol->next;
    }
    sol = sol->next;
  }
}

void Bell::DisclaimDP(int bag) {
  cerr << "Disclaiming DP for bag " << bag << endl;
  Trie* sol = dp[bag]->next;
  int i = 0;
  while(sol != NULL && (i++ < 5)) {
    cerr << "coloring = " << vec_printer(sol->colors) << ", val = " << sol->val
         << " | edges: " << edge_printer(sol->edges) << endl;
    sol = sol->next;
  }
}

static bool IsValidRoot(vector<int>& coloring) {
  unordered_set<int> cs;
  for(int i = 1; i < coloring.size(); i++) {
    if(coloring[i]) {
      cs.insert(coloring[i]);
    }
    if(cs.size() > 1) {
      return false;  
    }
  }
  //cerr << "cs of " << vec_printer(coloring) << " is " << cs.size()
  //     << " | Is valid? Answer: " << (cs.size() == 1) << endl;
  return true;
}

Trie* Bell::RootSolution(int bag) {
  Trie* sol = dp[bag]->next;  
  Trie* res = NULL;
  while(sol != NULL) {
    if(IsValidRoot(sol->colors) && (res == NULL || res->val > sol->val)) {
      res = sol;
    }
    //cerr << "current best solution is: " << vec_printer(sol->colors) << ", val = " << sol->val
    //     << " | edges: " << edge_printer(sol->edges) << endl;

    sol = sol->next;
  }
  return res;
}
