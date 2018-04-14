#ifndef H_TREE_DECOMPOSITION_
#define H_TREE_DECOMPOSITION_

#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;

struct TreeDecomposition {
   vector<set<int>> bags;
   vector<set<int>> inv_bags;
   vector<set<int>> tree;

   int removeSubsetBags();
   int maxBagSize();
   
   vector<vector<int>> getBags();
   vector<vector<int>> getTree();
};

#endif
