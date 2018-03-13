#include "instance.h"

int DynamicGraph::createNode(bool terminal) {
   Node node;
   node.id = next_node_id++;
   node.terminal = terminal;
   nodes[node.id] = node;

   return node.id;
}

int DynamicGraph::createEdge(int a, int b, int w) {
   Edge edge;
   edge.id = next_edge_id++;
   edge.endpoints[0] = a;
   edge.endpoints[1] = b;
   edge.weight = w;
   edges[edge.id] = edge;

   nodes[edge.endpoints[0]].edges.insert(edge.id);
   nodes[edge.endpoints[1]].edges.insert(edge.id);

   return edge.id;
}

Edge DynamicGraph::removeEdge(int id) {
   assert(edges.count(id));
   Edge edge = edges[id];
   edges.erase(id);
   nodes[edge.endpoints[0]].edges.erase(edge.id);
   nodes[edge.endpoints[1]].edges.erase(edge.id);
   return edge;
}

void DynamicGraph::addEdge(Edge edge) {
   assert(not edges.count(edge.id));
   edges[edge.id] = edge;

   nodes[edge.endpoints[0]].edges.insert(edge.id);
   nodes[edge.endpoints[1]].edges.insert(edge.id);

}

Node DynamicGraph::removeNode(int id) {
   assert(nodes.count(id));
   assert(nodes[id].edges.empty());

   Node node = nodes[id];
   nodes.erase(id);
   return node;
}

void DynamicGraph::addNode(Node node) {
   assert(not nodes.count(node.id));
   nodes[node.id] = node;
}


int DynamicGraph::neighbour(int node, Edge& edge) {
   if (edge.endpoints[0] == node) return edge.endpoints[1];
   if (edge.endpoints[1] == node) return edge.endpoints[0];
   return -1;
}
