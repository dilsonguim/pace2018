#ifndef _H_FLY_DSU_H_
#define _H_FLY_DSU_H_

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

struct FlyDSU {
  FlyDSU(int size) {
    pos = 0;
    int i = 0;
    while (i < size) {
      sets.push_back(i);
      mins.push_back(i);
      i++;
    }
    sizes.resize(size, 1);
    ops.resize(size);
    ops_mins.resize(size);
  }

  int Union(int a, int b);
  int Find(int a);
  void Undo();

  vector<int> sets;
  vector<int> sizes;
  vector<int> mins;
  // ops[i].first was merged into ops[i].second.
  int pos;
  vector<pair<int, int>> ops;
  vector<pair<int, int>> ops_mins;
};

#endif
