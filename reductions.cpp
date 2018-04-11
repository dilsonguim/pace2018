#include "nice_tree/draw_nice.h"
#include "reductions.h"
#include "trie/brute_force.h"

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
    cerr << "deg 1: " << node.id << endl;

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

    solution = reduceAndSolve(instance);
    for (auto& sol_edge : solution) {
      if (sol_edge.id == uv_id) {
        sol_edge = e[0];
        solution.push_back(e[1]);
        break;
      }
    }

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

  // if (degreeOneTest(instance, solution)) return solution;
  // if (nonTerminalDegreeTwoTest(instance, solution)) return solution;
  // if (parallelEdgeTest(instance, solution)) return solution;

  // cerr << "Irreducible n = " << instance->graph.nodes.size() <<
  //     " m = " << instance->graph.edges.size() << endl;

  auto& g = instance->graph;
  int n = 0;

  vector<bool> is_terminal;
  is_terminal.push_back(false);

  map<int, int> new_node_id;
  for (auto& id_node_pair : g.nodes) {
    auto& node = id_node_pair.second;
    n++;
    // cerr << "node.id: " << node.id << ", n: " << n << endl;
    new_node_id[node.id] = n;
    is_terminal.push_back(node.terminal);
  }

  map<int, int> new_bag_id;
  vector<vector<int>> bags(1);
  int max_bag_size = 0;
  // cerr << "bags_size = " << instance->bags.size() << endl;
  for (int old_bag_id = 0; old_bag_id < instance->bags.size(); old_bag_id++) {
    auto& old_bag = instance->bags[old_bag_id];
    vector<int> new_bag;
    for (auto& id : old_bag) {
      if (not new_node_id.count(id))
        continue;
      new_bag.push_back(new_node_id[id]);
    }
    if (new_bag.empty())
      continue;
    new_bag_id[old_bag_id] = bags.size();
    bags.push_back(new_bag);
    max_bag_size = max(max_bag_size, int(new_bag.size()));
  }

  vector<vector<int>> tree(bags.size());
  for (int i = 0; i < instance->tree.size(); i++) {
    if (not new_bag_id.count(i))
      continue;
    for (int j : instance->tree[i]) {
      if (not new_bag_id.count(j))
        continue;
      tree[new_bag_id[i]].push_back(new_bag_id[j]);
    }
  }

  Nice nicefier;
  auto nice = nicefier.getNiceTree(tree, bags, is_terminal, max_bag_size);
  cerr << "Nice decomposition has " << nice.tree.size() << " bags" << endl;

  Bell solver;
  solver.graph.resize(n + 1);
  solver.is_terminal = is_terminal;
  solver.tree.resize(nice.tree.size());
  dfs_tree(solver, nice.tree, nice.root, -1);

  solver.bags = nice.bags;
  for (auto& id_edge_pair : instance->graph.edges) {
    auto& e = id_edge_pair.second;
    int w = e.weight;

    int a = new_node_id[e.endpoints[0]];
    int b = new_node_id[e.endpoints[1]];
    solver.graph[a].push_back({ b, w });
    solver.graph[b].push_back({ a, w });
  }

  for (auto& bag : solver.bags) {
    sort(bag.begin(), bag.end());
  }
  for (auto& adj : solver.graph) {
    sort(adj.begin(), adj.end());
  }
  solver.dp.resize(solver.bags.size());

  Trie* sol = NULL;
  if (!solver.Solve(nice.root)) {
    cerr << "Existem terminais em mais de uma componente conexa" << endl;
  }
  else {
    sol = solver.RootSolution(nice.root);
  }

  bool file_print = true;
  if (file_print) {
    {
      ofstream file("decomposition.dot");
      Draw(solver.tree, solver.bags, solver.is_terminal, file);
      file.close();
    }

    if (true) {
      ofstream file("solution.dot");
      vector<vector<pair<int, int>>> sol_graph(solver.graph.size());
      for (auto& e : sol->edges) {
        int w = 0;
        for (auto& x : solver.graph[e[0]]) {
          w = (x.first == e[1]) ? x.second : w;
        }
        sol_graph[e[0]].push_back({ e[1], w });
        sol_graph[e[1]].push_back({ e[0], w });
      }
      DrawGraph(sol_graph, solver.is_terminal, file);
      file.close();
    }

    {
      ofstream file("graph.dot");
      DrawGraph(solver.graph, solver.is_terminal, file);
      file.close();
    }
  }
  bool debug = true;
  if (!debug) {
    cout << "VALUE " << sol->val << endl;
    for (auto& e : sol->edges) {
      cout << e[0] << " " << e[1] << endl;
    }
    return solution;
  }

  cerr << "Solution value is " << sol->val << endl;
  for (auto& e : sol->edges) {
    cerr << e[0] << " " << e[1] << endl;
  }

  unique_ptr<Solution> brute_sol(BruteForceSolve(solver));
  cerr << "Brute force solution value is " << brute_sol->val << endl;
  for (auto& e : brute_sol->edges) {
    cerr << e[0] << " " << e[1] << endl;
  }

  if (false) {
    cerr << "verificando se eh tree decomposition" << endl;

    if (nicefier.isTD(nice.tree, nice.bags, is_terminal, nice.root,
                      solver.graph)) {
      cerr << "eh tree decompositon" << endl;
    } else {
      cerr << "WARNING!!!!!! :X" << endl;
      cerr << "nao eh tree decompositon" << endl;
    }

    cerr << "verificando se tem as propriedades nice" << endl;

    if (nicefier.isNice(nice.tree, nice.bags, is_terminal, nice.root)) {
      cerr << "esta nice" << endl;
    } else {
      cerr << "WARNING!!!!!! :X" << endl;
      cerr << "nao eh nice decomposition" << endl;
    }

    nicefier.Debug();
  }
  return solution;
}
