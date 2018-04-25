#ifndef H_RELAXATION_
#define H_RELAXATION_

#include <cmath>
#include "instance.h"

struct Relaxation {

   int n, m;
   vector<int> terminals;
   vector<vector<pair<int, int>>> g; // (neighbour, edge index)
   vector<pair<int, int>> edges;
   vector<long long> weight;
   vector<int> fixed_to_zero;
   vector<int> fixed_to_one;
   vector<vector<double>> lag_cost;
   vector<vector<int>> sol;
   vector<vector<double>> mul;
   vector<vector<double>> sg;


   vector<double> dst;
   vector<int> edge_to_parent;

   Relaxation(const vector<vector<pair<int, int>>>& graph, const vector<bool>& is_terminal);

   void computeLagrangianCost();
   double minCostToConnect(const vector<double>& cost, int a, int b,
                           vector<int>& sol);
   double solveSubproblem();
   double computeSubgradient();

   void run();
};

#endif
