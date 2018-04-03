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

struct DSU {
  DSU(int size) {
    int i = 0;
    while(i < size) {
      sets.push_back(i++);
    }
    sizes.resize(size, 1);
  }  

  int Union(int a, int b);
  int Find(int a);

  vector<int> sets;
  vector<int> sizes;
};


