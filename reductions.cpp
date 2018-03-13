#include "reductions.h"


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
      if (node.edges.size() != 1) continue;

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

      if (node.terminal) continue;
      if (node.edges.size() != 2) continue;
      
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

bool parallelEdgeTest(Instance * instance, vector<Edge>& solution) {
   auto& g = instance->graph;
   map<pair<int, int>, vector<int>> groups;
   for (auto& id_edge_pair : g.edges) {
      auto& e = id_edge_pair.second;
      int u = e.endpoints[0];
      int v = e.endpoints[1];
      if (u > v) swap(u, v);
      groups[{u,v}].push_back(e.id);
   }

   vector<Edge> removed_edges;
   for (auto& group : groups) {
      if (group.second.size() < 2) continue;
      int keep = group.second[0];
      for (auto id : group.second) {
         if (g.edges[id].weight < g.edges[keep].weight) {
            keep = id;
         }
      }
      for (auto id : group.second) {
         if (id == keep) continue;
         removed_edges.push_back(g.removeEdge(id));
      }
   }

   if (removed_edges.empty()) return false;

   solution = reduceAndSolve(instance);

   for (auto& e : removed_edges) {
      g.addEdge(e);
   }

   return true;
}

vector<Edge> reduceAndSolve(Instance* instance) {
   vector<Edge> solution;

   if (degreeOneTest(instance, solution)) return solution;
   if (nonTerminalDegreeTwoTest(instance, solution)) return solution;
   if (parallelEdgeTest(instance, solution)) return solution;


   //Call solve here
   cout << "Irreducible n = " << instance->graph.nodes.size() <<
        " m = " << instance->graph.edges.size() << endl;

   return solution;
}


