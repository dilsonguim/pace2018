#ifndef _H_BRUTE_FORCE_H
#define _H_BRUTE_FORCE_H

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
#include "bell_dp.h"

using namespace std;

struct Solution {
  vector<vector<int>> edges; //(u,v,weight)
  long long val;
};


Solution* BruteForceSolve(Bell& bell);

#endif
