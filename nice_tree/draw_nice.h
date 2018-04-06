#ifndef _H_DRAW_NICE_H_
#define _H_DRAW_NICE_H_

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
#include <memory>

using namespace std;

void Draw(int root, vector<vector<int>>& tree, vector<vector<int>>& bags,
          vector<bool>& is_terminal, ostream& out);

void DrawGraph(vector<vector<pair<int, int>>>& graph,
               vector<bool>& is_terminal, ostream& out);
#endif

