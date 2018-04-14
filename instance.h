#ifndef H_INSTANCE_
#define H_INSTANCE_

#include "tree_decomposition.h"
#include <algorithm>
#include <cassert>
#include <map>
#include <set>
#include <vector>

#include <iostream>

using namespace std;


struct Node {
   int id;
   bool terminal;

   set<int> edges;
};

struct Edge {
   int id;
   int endpoints[2];
   int weight;
};

struct DynamicGraph {
   int next_node_id;
   int next_edge_id;
   map<int, Node> nodes;
   map<int, Edge> edges;

   DynamicGraph() : next_node_id(0), next_edge_id(0) {}

   int createNode(bool terminal);
   int createEdge(int a, int b, int w);

   Edge removeEdge(int id);
   void addEdge(Edge edge);

   Node removeNode(int id);
   void addNode(Node node);

   int neighbour(int node, Edge& edge);

   map<int, int> remapNodeIds();
   
   vector<bool> getRemappedTerminals(const map<int, int>& new_node_id);

   vector<vector<pair<int, int>>> getRemappedGraph(
         const map<int, int>& new_node_id);
};

struct DynamicTreeDecomposition {
   vector<set<int>> bags;
   vector<vector<int>> tree;

   struct ReplacementUndoData {
      vector<pair<int, int>> add;
      vector<pair<int, int>> remove;
   };

   ReplacementUndoData replaceNode(int a, int b);

   void undoReplaceNode(const ReplacementUndoData& undo_data);

   TreeDecomposition getRemappedTreeDecomposition(
         const map<int, int>& new_node_id);
};

struct Instance {

   DynamicGraph graph;
   
   DynamicTreeDecomposition td;
};

#endif
