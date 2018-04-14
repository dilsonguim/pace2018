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
   assert(nodes[edge.endpoints[0]].edges.count(edge.id));
   assert(nodes[edge.endpoints[1]].edges.count(edge.id));
   nodes[edge.endpoints[0]].edges.erase(edge.id);
   nodes[edge.endpoints[1]].edges.erase(edge.id);
   return edge;
}

void DynamicGraph::addEdge(Edge edge) {
   assert(not edges.count(edge.id));
   edges[edge.id] = edge;

   assert(nodes.count(edge.endpoints[0]));
   assert(nodes.count(edge.endpoints[1]));
   assert(not nodes[edge.endpoints[0]].edges.count(edge.id));
   assert(not nodes[edge.endpoints[1]].edges.count(edge.id));
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
   assert(node.edges.empty());
   nodes[node.id] = node;
}


int DynamicGraph::neighbour(int node, Edge& edge) {
   if (edge.endpoints[0] == node) return edge.endpoints[1];
   if (edge.endpoints[1] == node) return edge.endpoints[0];
   assert(false);
   return -1;
}

DynamicTreeDecomposition::ReplacementUndoData 
DynamicTreeDecomposition::replaceNode(int a, int b) {
   ReplacementUndoData undo_data;

   for (int i = 0; i < bags.size(); i++) {
      if (not bags[i].count(a)) continue;

      bags[i].erase(a);
      undo_data.add.push_back({i, a});

      if (not bags[i].count(b)) {
         bags[i].insert(b);
         undo_data.remove.push_back({i, b});
      }
   }

   return undo_data;
}

map<int, int> DynamicGraph::remapNodeIds() {
   map<int, int> new_node_id;
   int n = 0;
   for (auto& id_node_pair : nodes) {
      auto& node = id_node_pair.second;
      n++;
      new_node_id[node.id] = n;
   }
   return new_node_id;
}
   
vector<bool> DynamicGraph::getRemappedTerminals(const map<int, int>& new_node_id)
                                           {
   vector<bool> is_terminal(nodes.size() + 1);
   for (auto& id_node_pair : nodes) {
      auto& node = id_node_pair.second;
      if (node.terminal) {
         is_terminal[new_node_id.at(node.id)] = true;
      }
   }

   return is_terminal;
}

vector<vector<pair<int, int>>> DynamicGraph::getRemappedGraph(
      const map<int, int>& new_node_id) {

   vector<vector<pair<int, int>>> graph(nodes.size() + 1);

   for (auto& id_edge_pair : edges) {
      auto& e = id_edge_pair.second;
      int w = e.weight;

      int a = new_node_id.at(e.endpoints[0]);
      int b = new_node_id.at(e.endpoints[1]);
      graph[a].push_back({ b, w });
      graph[b].push_back({ a, w });
   }

   for (auto& adj : graph) {
      sort(adj.begin(), adj.end());
   }
   
   return graph;
}

TreeDecomposition DynamicTreeDecomposition::getRemappedTreeDecomposition(
     const map<int, int>& new_node_id) {

   TreeDecomposition td;
   td.bags.push_back(set<int>());
   td.inv_bags.resize(new_node_id.size() + 1);

   map<int, int> new_bag_id;
   for (int old_bag_id = 0; old_bag_id < bags.size(); old_bag_id++) {
      set<int> new_bag;
      for (auto& id : bags[old_bag_id]) {
         if (new_node_id.count(id)) {
            new_bag.insert(new_node_id.at(id));
         }
      }
      if (not new_bag.empty()) {

         int id = new_bag_id[old_bag_id] = td.bags.size();
         
         for (auto node : new_bag) {
            td.inv_bags[node].insert(id);
         }
         td.bags.push_back(new_bag);
      }
   }


   td.tree.resize(td.bags.size());
   for (int i = 0; i < tree.size(); i++) {
      if (not new_bag_id.count(i)) continue;

      for (int j : tree[i]) {
         if (not new_bag_id.count(j)) continue;
         td.tree[new_bag_id.at(i)].insert(new_bag_id.at(j));
      }
   }

   return td;
}


void DynamicTreeDecomposition::undoReplaceNode(
      const ReplacementUndoData& undo_data) {

   for (auto op : undo_data.remove) {
      bags[op.first].erase(op.second);
   }

   for (auto op : undo_data.add) {
      bags[op.first].insert(op.second);
   }
}
