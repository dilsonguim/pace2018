#include "nice_tree/draw_nice.h"
#include "reductions.h"
#include "trie/brute_force.h"
#include "trie/order_optimizer.h"

/*******************************************************************************
The implementation of a reduction consists of a function that gets a pointer
to an instance, a reference to vector where to store a solution and returns a
boolean indicating whether the reduction was successfully applied.

In positive case, a solution must be computed by the reduction function.
Usually, reductions will compute solutions by calling reduceAndSolve in a
modified instance and applying necessary modifications to this solution.

See degreeOneTest for example of a reduction that gets rid of degree one nodes.

*******************************************************************************/

bool degreeOneTest(Instance* instance, vector<Edge>& solution) {
  auto& g = instance->graph;
  for (auto& id_node_pair : g.nodes) {
    auto& node = id_node_pair.second;
    if (node.edges.size() != 1)
      continue;

    Edge e = g.removeEdge(*node.edges.begin());
    int v = g.neighbour(node.id, e);
    Node u = g.removeNode(node.id);
    bool was_terminal = g.nodes[v].terminal;
    if (u.terminal) {
      g.nodes[v].terminal = true;
    }

    solution = reduceAndSolve(instance);
    if (u.terminal) {
      solution.push_back(e);
    }

    g.nodes[v].terminal = was_terminal;
    g.addNode(u);
    g.addEdge(e);

    return true;
  }
  return false;
}

bool nonTerminalDegreeTwoTest(Instance* instance, vector<Edge>& solution) {
  auto& g = instance->graph;
  for (auto& id_node_pair : g.nodes) {
    auto& node = id_node_pair.second;

    if (node.terminal)
      continue;
    if (node.edges.size() != 2)
      continue;

    vector<Edge> e;
    e.push_back(g.removeEdge(*node.edges.begin()));
    e.push_back(g.removeEdge(*node.edges.begin()));
    Node mid = g.removeNode(node.id);

    int u = g.neighbour(mid.id, e[0]);
    int v = g.neighbour(mid.id, e[1]);
    int uv_id = g.createEdge(u, v, e[0].weight + e[1].weight);
    auto td_undo_data = instance->td.replaceNode(mid.id, u);

    solution = reduceAndSolve(instance);
    for (auto& sol_edge : solution) {
      if (sol_edge.id == uv_id) {
        sol_edge = e[0];
        solution.push_back(e[1]);
        break;
      }
    }

    instance->td.undoReplaceNode(td_undo_data);
    g.removeEdge(uv_id);
    g.addNode(mid);
    g.addEdge(e[0]);
    g.addEdge(e[1]);

    return true;
  }

  return false;
}

bool parallelEdgeTest(Instance* instance, vector<Edge>& solution) {
  auto& g = instance->graph;
  map<pair<int, int>, vector<int>> groups;
  for (auto& id_edge_pair : g.edges) {
    auto& e = id_edge_pair.second;
    int u = e.endpoints[0];
    int v = e.endpoints[1];
    if (u > v)
      swap(u, v);
    groups[{ u, v }].push_back(e.id);
  }

  vector<Edge> removed_edges;
  for (auto& group : groups) {
    if (group.second.size() < 2)
      continue;
    int keep = group.second[0];
    for (auto id : group.second) {
      if (g.edges[id].weight < g.edges[keep].weight) {
        keep = id;
      }
    }
    for (auto id : group.second) {
      if (id == keep)
        continue;
      removed_edges.push_back(g.removeEdge(id));
    }
  }

  if (removed_edges.empty())
    return false;

  solution = reduceAndSolve(instance);

  for (auto& e : removed_edges) {
    g.addEdge(e);
  }

  return true;
}

void dijkstra(vector<vector<pair<int, int>>>& g, vector<long long>& d,
              int start) {

   fill(d.begin(), d.end(), numeric_limits<long long>::max());
   set<pair<long long, int>> q;
   d[start] = 0;
   q.insert({d[start], start});
   while (not q.empty()) {
      int a = q.begin()->second;
      q.erase(q.begin());

      for (const auto& e : g[a]) {
         const int b = e.first;
         const long long w = e.second;


         if (d[a] + w < d[b]) {
            if (d[b] < numeric_limits<long long>::max()) {
               q.erase({d[b], b});
            }
            d[b] = d[a] + w;
            q.insert({d[b], b});
         }
      }
   }
}

struct SpecialDsf {
   vector<int> parent;
   vector<vector<int>> sets;

   SpecialDsf(int n) {
      parent.resize(n);
      sets.resize(n);
      for (int i = 0; i < n; i++) {
         parent[i] = i;
         sets[i].push_back(i);
      }
   }

   int find(int x) {
      if (parent[x] != x) {
         parent[x] = find(parent[x]);
      }
      return parent[x];
   }

   void join(int x, int y) {
      int px = find(x);
      int py = find(y);

      if (sets[px].size() > sets[py].size()) {
         swap(px, py);
      }

      parent[px] = py;
      for (auto i : sets[px]) {
         sets[py].push_back(i);
      }
      sets[px].clear();
   }
};

bool specialCostTest(Instance* instance, vector<Edge>& solution) {
  auto new_node_id = instance->graph.remapNodeIds();
  auto is_terminal = instance->graph.getRemappedTerminals(new_node_id);
  auto g = instance->graph.getRemappedGraph(new_node_id);

  vector<vector<long long>> d(g.size());
  for (int i = 1; i < g.size(); i++) {
     if (is_terminal[i]) {
        d[i].resize(g.size());
        dijkstra(g, d[i], i);
     }
  }

  vector<pair<long long, pair<int, int>>> join_order;
  for (int i = 1; i < g.size(); i++) {
     if (not is_terminal[i]) continue;

     for (int j = 1; j < g.size(); j++) {
        if (i == j) continue;
        if (not is_terminal[j]) continue;

        join_order.push_back({d[i][j], {i, j}});
     }
  }
  sort(join_order.begin(), join_order.end());
  SpecialDsf dsf(g.size());
  for (const auto& join : join_order) {
     const int a = join.second.first;
     const int b = join.second.second;
     const long long sd = join.first;

     int id_a = dsf.find(a);
     int id_b = dsf.find(b);
     if (id_a == id_b) continue;

     for (int p : dsf.sets[id_a]) {
        for (int q : dsf.sets[id_b]) {
           d[p][q] = sd;
           d[q][p] = sd;
        }
     }

     dsf.join(id_a, id_b);
  }



  vector<int> edges_to_remove;
  for (const auto& id_edge_pair : instance->graph.edges) {
     const auto& edge = id_edge_pair.second;
     const int a = new_node_id.at(edge.endpoints[0]);
     const int b = new_node_id.at(edge.endpoints[1]);
     const int w = edge.weight;
     
     bool suboptimal = false;
     for (int k1 = 1; k1 < g.size() and not suboptimal; k1++) {
        if (not is_terminal[k1]) continue;
        if (d[k1][a] >= w) continue;
        for (int k2 = 1; k2 < g.size(); k2++) {
           if (not is_terminal[k2]) continue;
           if (d[k2][b] >= w or d[k1][k2] >= w) continue;
           suboptimal = true;
           break;
        }
     }
     
     if (suboptimal) {
        edges_to_remove.push_back(id_edge_pair.first);
     }
  }
  
  
  
  sort(edges_to_remove.begin(), edges_to_remove.end());
  edges_to_remove.resize(distance(edges_to_remove.begin(),
                                  unique(edges_to_remove.begin(),
                                         edges_to_remove.end())));

  if (edges_to_remove.empty()) return false;

  vector<Edge> removed_edges;
  for (auto id : edges_to_remove) {
     removed_edges.push_back(instance->graph.removeEdge(id));
  }
  solution = reduceAndSolve(instance);
  for (auto& edge : removed_edges) {
     instance->graph.addEdge(edge);
  }

  return true;
}

bool bottleneckTest(Instance* instance, vector<Edge>& solution) {
   auto new_node_id = instance->graph.remapNodeIds();
   auto is_terminal = instance->graph.getRemappedTerminals(new_node_id);
   auto g = instance->graph.getRemappedGraph(new_node_id);
   
   
   vector<vector<long long>> d(g.size());
   for (int i = 1; i < g.size(); i++) {
      if (is_terminal[i]) {
         d[i].resize(g.size());
         dijkstra(g, d[i], i);
      }
   }

   map<pair<int, int>, int> edge_id;
   vector<pair<int, pair<int, int>>> joins;
   for (const auto& id_edge_pair : instance->graph.edges) {
      const auto& edge = id_edge_pair.second;
      const int a = new_node_id.at(edge.endpoints[0]);
      const int b = new_node_id.at(edge.endpoints[1]);
      const int w = edge.weight;
      edge_id[{a, b}] = id_edge_pair.first;
      edge_id[{b, a}] = id_edge_pair.first;
      joins.push_back({w, {a, b}});
   }
   sort(joins.begin(), joins.end());

   SpecialDsf outer_dsf(g.size());
   for (int i = 1; i < g.size(); i++) {
      if (not is_terminal[i]) {
         outer_dsf.sets[i].clear();
      }
   }

   vector<int> edges_to_include;
   for (int join_i = 0; join_i < joins.size(); join_i++) {
      const int a_i = joins[join_i].second.first;
      const int b_i = joins[join_i].second.second;
      const int w_i = joins[join_i].first;

      if (outer_dsf.find(a_i) == outer_dsf.find(b_i)) continue;

      bool include = false;
      auto inner_dsf = outer_dsf;
      for (int join_j = join_i + 1; join_j < joins.size() and not include;
           join_j++) {

         const int a_j = joins[join_j].second.first;
         const int b_j = joins[join_j].second.second;
         const int w_j = joins[join_j].first;

         if (inner_dsf.find(a_j) == inner_dsf.find(b_j)) continue;

         for (int k1 : inner_dsf.sets[inner_dsf.find(a_j)]) {
            if (min(d[k1][a_i], d[k1][b_i]) + w_i > w_j) continue;
            for (int k2 : inner_dsf.sets[inner_dsf.find(b_j)]) {
               if (d[k1][a_i] + d[k2][b_i] + w_i <= w_j or
                   d[k1][b_i] + d[k2][a_i] + w_i <= w_j) {

                  include = true;
                  break;
               }
            }
            if (include) break;
         }

         inner_dsf.join(a_j, b_j);
      }
      if (include) {
         edges_to_include.push_back(edge_id.at({a_i, b_i}));
      }


      outer_dsf.join(a_i, b_i);
   }


   if (edges_to_include.empty()) return false;

   auto& graph = instance->graph;

   vector<Edge> edges_to_put_back;
   vector<Node> nodes_to_put_back;
   vector<Edge> edges_to_add_to_the_solution;
   vector<DynamicTreeDecomposition::ReplacementUndoData> td_undo;
   vector<int> edges_to_remove;
   vector<int> make_non_terminal;
   map<int, Edge> origin_edge;

   SpecialDsf remap_dsf(g.size());
   for (int i = 1; i < g.size(); i++) {
      if (not is_terminal[i]) {
         remap_dsf.sets[i].clear();
      }
   }
   for (int id : edges_to_include) {
      auto edge = graph.removeEdge(id);
      edges_to_put_back.push_back(edge);
      edges_to_add_to_the_solution.push_back(edge);
      const int a = new_node_id.at(edge.endpoints[0]);
      const int b = new_node_id.at(edge.endpoints[1]);
      assert(remap_dsf.find(a) != remap_dsf.find(b));
      remap_dsf.join(a, b);
   }

   vector<int> old_node_id(g.size());
   for (const auto& mapping : new_node_id) {
      old_node_id[mapping.second] = mapping.first;
   }

   vector<int> touched_edges;
   vector<Edge>  reallocated_edges;
   vector<Edge> origin;
   for (auto& id_edge_pair : graph.edges) {
      auto& edge = id_edge_pair.second;
      const int a = new_node_id.at(edge.endpoints[0]);
      const int b = new_node_id.at(edge.endpoints[1]);
      const int ra = remap_dsf.find(a);
      const int rb = remap_dsf.find(b);

      if (ra == a and rb == b) continue;

      touched_edges.push_back(edge.id);

      if (ra == rb) continue;

      Edge re;
      re.endpoints[0] = old_node_id[ra];
      re.endpoints[1] = old_node_id[rb];
      re.weight = edge.weight;
      reallocated_edges.push_back(re);
      origin.push_back(edge);
   }
   for (int id : touched_edges) {
      edges_to_put_back.push_back(graph.removeEdge(id));
   }

   for (int i = 0; i < reallocated_edges.size(); i++) {
      auto& edge = reallocated_edges[i];
      edge.id = graph.createEdge(edge.endpoints[0], edge.endpoints[1],
                                 edge.weight);
      edges_to_remove.push_back(edge.id);
      origin_edge[edge.id] = origin[i];
   }

   for (int i = 1; i < g.size(); i++) {
      int ri = remap_dsf.find(i);
      if (ri == i) {

         if (not is_terminal[i] and not remap_dsf.sets[ri].empty()) {
            int id = old_node_id[i];
            make_non_terminal.push_back(id);
            graph.nodes[id].terminal = true;
         }

         continue;
      }

      int id_i = old_node_id[i];
      int id_ri = old_node_id[ri];

      td_undo.push_back(instance->td.replaceNode(id_i, id_ri));
      
      nodes_to_put_back.push_back(graph.removeNode(id_i));
   }
   
   solution = reduceAndSolve(instance);

   for (auto& edge : solution) {
      if (origin_edge.count(edge.id)) {
         edge = origin_edge.at(edge.id);
      }
   }

   for (auto& edge : edges_to_add_to_the_solution) {
      solution.push_back(edge);
   }

   for (int id : edges_to_remove) {
      graph.removeEdge(id);
   }

   for (auto& node : nodes_to_put_back) {
      graph.addNode(node);
   }

   for (auto& undo_data : td_undo) {
      instance->td.undoReplaceNode(undo_data);
   }

   for (auto& edge : edges_to_put_back) {
      graph.addEdge(edge);
   }

   for (int id : make_non_terminal) {
      graph.nodes[id].terminal = false;
   }

   return true;
}

static void dfs_tree(Bell& solver, vector<vector<int>>& tree, int a, int p) {
  for (auto& b : tree[a]) {
    if (b == p)
      continue;
    solver.tree[a].push_back(b);
    dfs_tree(solver, tree, b, a);
  }
}

vector<Edge> reduceAndSolve(Instance* instance) {
  vector<Edge> solution;

  if (degreeOneTest(instance, solution)) return solution;
  if (nonTerminalDegreeTwoTest(instance, solution)) return solution;
  if (parallelEdgeTest(instance, solution)) return solution;
  if (specialCostTest(instance, solution)) return solution;
  if (bottleneckTest(instance, solution)) return solution;
  if (parallelEdgeTest(instance, solution)) return solution;
  // cerr << "Irreducible n = " << instance->graph.nodes.size() <<
  //     " m = " << instance->graph.edges.size() << endl;

  Bell solver;
  auto& g = instance->graph;
  map<int, int> new_node_id = g.remapNodeIds();
  solver.is_terminal = g.getRemappedTerminals(new_node_id);
  solver.graph = g.getRemappedGraph(new_node_id);

  /*
  cout << "Graph:" << endl;
  for (int i = 1; i < solver.graph.size(); i++) {
     cout << i << ":";
     for (auto e : solver.graph[i]) {
        cout << " (" << e.first << ", " << e.second << ")";
     }
     cout << endl;
  }
  cout << endl;
  */

  TreeDecomposition td = instance->td.getRemappedTreeDecomposition(new_node_id);
  td.cleanDecomposition(solver.graph);
  auto bags = td.getBags();
  auto tree = td.getTree();
  if (bags.size() == 1) {
     bags.push_back(vector<int>(1, 1));
     tree.push_back(vector<int>());
  }

  /*
  cout << "Bags:" << endl;
  for (int i = 1; i < bags.size(); i++) {
     cout << i << ":";
     for (auto node : bags[i]) {
        cout << ' ' << node;
     }
     cout << endl;
  }
  cout << endl;

  cout << "Tree:" << endl;
  for (int i = 1; i < tree.size(); i++) {
     cout << i << ":";
     for (int j : tree[i]) {
        cout << ' ' << j;
     }
     cout << endl;
  }
  cout << endl;

  Nice checker;
  assert(checker.isTD(tree, bags, vector<bool>(solver.graph.size(), true), 1,
                      solver.graph));
  */

  Nice nicefier;
  auto nice = nicefier.getNiceTree(tree, bags, solver.is_terminal,
                                   td.maxBagSize());
  solver.tree.resize(nice.tree.size());
  dfs_tree(solver, nice.tree, nice.root, -1);
  solver.bags = nice.bags;
  for (auto& bag : solver.bags) {
    sort(bag.begin(), bag.end());
  }
  solver.dp.resize(solver.bags.size());

  OrderOptimizer order_optimizer(&solver.tree);
  order_optimizer.Solve(nice.root);
  order_optimizer.Optimize(nice.root);

  Trie* sol = NULL;
  if (!solver.Solve(nice.root)) {
    cerr << "Existem terminais em mais de uma componente conexa" << endl;
    exit(0);
  }
  else {
    sol = solver.RootSolution(nice.root);
  }
  sort(sol->edges.begin(), sol->edges.end());

  for (auto& id_edge_pair : instance->graph.edges) {
    auto& e = id_edge_pair.second;
    
    int a = new_node_id[e.endpoints[0]];
    int b = new_node_id[e.endpoints[1]];

    if (binary_search(sol->edges.begin(), sol->edges.end(), vector<int>{a, b})
       or binary_search(sol->edges.begin(), sol->edges.end(), vector<int>{b, a})
       ) {

       solution.push_back(e);
    }
  }


  
  return solution;
}
