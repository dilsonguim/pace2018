#ifndef _H_FLY_DSU_H_
#define _H_FLY_DSU_H_

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
#include <vector>

using namespace std;

struct FlyDSU {
  FlyDSU(int size) {
    int i = 0;
    while(i < size) {
      sets.push_back(i++);
    }
    sizes.resize(size, 1);
  }  

  int Union(int a, int b);
  int Find(int a);
  void Undo();

  vector<int> sets;
  vector<int> sizes;
  // ops[i].first was merged into ops[i].second.
  vector<pair<int,int>> ops;
};


#endif

