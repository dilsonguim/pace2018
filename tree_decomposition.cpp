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

