#include "tree_decomposition.h"

bool isSubset(const set<int>& a, const set<int>& b) {
   if (b.size() < a.size()) return false;
   for (int i : a) {
      if (not b.count(i)) {
         return false;
      }
   }
   return true;
}

int TreeDecomposition::removeSubsetBags() {
   int reductions = 0;
   for (int i = 1; i < bags.size(); i++) {
      if (bags[i].empty()) continue;

      for (int j : tree[i]) {
         if (not isSubset(bags[i], bags[j])) continue;

         reductions++;

         for (int k : tree[i]) {
            tree[k].erase(i);

            if (k != j) {
               tree[k].insert(j);
               tree[j].insert(k);
            }
         }

         tree[i].clear();
         for (auto& node : bags[i]) {
            inv_bags[node].erase(i);
         }
         bags[i].clear();

         break;
      }
   }

   return reductions;
}

int TreeDecomposition::maxBagSize() {
   int max_bag_size = 0;
   for (auto& bag : bags) {
      max_bag_size = max(max_bag_size, int(bag.size()));
   }
   return max_bag_size;
}
   
vector<vector<int>> TreeDecomposition::getBags() {
   vector<vector<int>> nonempty_bags(1);
   for (const auto& bag : bags) {
      if (not bag.empty()) {
         nonempty_bags.push_back(vector<int>(bag.begin(), bag.end()));
      }
   }
   return nonempty_bags;
}


vector<vector<int>> TreeDecomposition::getTree() {
   map<int, int> new_id;
   int next_id = 1;
   for (int i = 1; i < bags.size(); i++) {
      if (not bags[i].empty()) {
         new_id[i] = next_id++;
      }
   }

   vector<vector<int>> remaining_tree(next_id);
   for (int i = 1; i < tree.size(); i++) {
      if (not new_id.count(i)) continue;

      for (int j : tree[i]) {
         if (new_id.count(j)) {
            remaining_tree[new_id.at(i)].push_back(new_id.at(j));
         }
      }
   }

   return remaining_tree;
}


int TreeDecomposition::clearBags(const vector<vector<pair<int, int>>>& graph) {
   int operations = 0;
   vector<int> order(bags.size() - 1);
   for (int i = 1; i < bags.size(); i++) {
      order[i - 1] = i;
   }
   sort(order.begin(), order.end(), [&](int i, int j) {
                                       return bags[i].size() > bags[j].size();
                                    });


   for (auto bag_id : order) {
      for (int i : bags[bag_id]) {
         int ngb_count = 0;
         for (int ngb_bag : tree[bag_id]) {
            if (bags[ngb_bag].count(i)) {
               ngb_count++;
               if (ngb_count > 1) break;
            }
         }
         if (ngb_count > 1) continue;

         
         bool can_remove = true;
         for (const auto& e : graph[i]) {
            int j = e.first;

            bool represented_elsewhere = false;
            for (int k : inv_bags[i]) {
               if (k == bag_id) continue;
               if (not inv_bags[j].count(k)) continue;
               represented_elsewhere = true;
               break;
            }
            if (not represented_elsewhere) {
               can_remove = false;
               break;
            }
         }

         if (can_remove) {
            bags[bag_id].erase(i);
            inv_bags[i].erase(bag_id);
            operations++;
            break;
         }
      }
   }

   return operations;
}
   
void TreeDecomposition::cleanDecomposition(
      const vector<vector<pair<int, int>>>& graph) {

   do {
      removeSubsetBags();
   } while(clearBags(graph));
}
