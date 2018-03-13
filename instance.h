#ifndef H_INSTANCE_
#define H_INSTANCE_

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
};

struct Instance {

   DynamicGraph graph;

   //Tree decomposition
   vector<vector<int>> bags;
   vector<vector<int>> tree;

   void addNode(int id);
   void removeNode(int id);

   void addEdge(int a, int b, int w);
   void removeEdge(int a, int b);

   bool hasNode(int id);

   //Removes node b from adj[a]
   void removeFromAdj(int a, int b);
};

#endif
