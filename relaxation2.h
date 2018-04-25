#ifndef H_RELAXATION2_
#define H_RELAXATION2_

#include <limits>
#include <vector>
#include <algorithm>
#include <iostream>
#include "instance.h"

using namespace std;

struct DSF {
   vector<int> p;

   DSF(int n);

   int Find(int x);
   void Join(int x, int y);
};

struct Relaxation2 {
   int n, m;
   vector<int> terminals;
   vector<vector<pair<int, int>>> g; // (neighbour, edge index)
   vector<pair<int, int>> edges;
   vector<long long> weight;
   vector<int> fixed_at_zero;
   vector<int> fixed_at_one;
   
   vector<double> lag_cost_x;
   vector<double> lag_cost_y;
   vector<pair<double, double>> mul;
   vector<pair<double, double>> sg;
   vector<int> sol_x;
   vector<int> sol_y;
   
   void ComputeLagrangianCost();
   double SolveSubproblem();
   double ComputeSubgradient();
   void Run();

   Relaxation2(const vector<vector<pair<int, int>>>& graph, const vector<bool>& is_terminal);
};

#endif
