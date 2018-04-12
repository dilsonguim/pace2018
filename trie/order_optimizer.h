#ifndef _H_ORDER_OPTIMIZER_H_
#define _H_ORDER_OPTIMIZER_H_

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <list>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using namespace std;

struct OrderOptimizer {
  OrderOptimizer(vector<vector<int>>* t) {
    tree = t;
    dp.resize(tree->size(), -1);
  }
  vector<vector<int>>* tree;
  vector<int> dp;

  int Solve(int i);
  
  void Optimize(int i);
};

#endif

