#include "relaxation.h"


double Relaxation::minCostToConnect(const vector<double>& cost, int a, int b,
                                    vector<int>& sol) {

   fill(sol.begin(), sol.end(), 0);
   double obj = 0.0;

   for (int i = 0; i < m; i++) {
      if (fixed_to_one[i] or cost[i] < 0.0) {
         obj += cost[i];
         sol[i] = 1;
      }
   }

   fill(dst.begin(), dst.end(), numeric_limits<double>::infinity());
   fill(edge_to_parent.begin(), edge_to_parent.end(), -1);
   set<pair<double, int>> q;
   dst[a] = 0;
   q.insert({dst[a], a});
   while (not q.empty()) {
      int u = q.begin()->second;
      q.erase(q.begin());
      if (u == b) break;

      for (const auto& e : g[u]) {
         const int v = e.first;
         const int id = e.second;
         if (fixed_to_zero[id]) continue;
         const double w = (fixed_to_one[id] or cost[id] < 0.0) ? 0.0 : cost[id];
         if (dst[u] + w < dst[v]) {
            if (dst[v] < numeric_limits<double>::infinity()) {
               q.erase({dst[v], v});
            }
            edge_to_parent[v] = id;
            dst[v] = dst[u] + w;
            q.insert({dst[v], v});
         }
      }
   }
   obj += dst[b];
   
   int h = b;
   while (edge_to_parent[h] != -1) {
      sol[edge_to_parent[h]] = 1;
      if (h == edges[edge_to_parent[h]].first) {
         h = edges[edge_to_parent[h]].second;
      } else {
         h = edges[edge_to_parent[h]].first;
      }
   }

   return obj;
}
   
void Relaxation::computeLagrangianCost() {
   for (int i = 1; i < terminals.size(); i++) {
      for (int j = 0; j < m; j++) {
         lag_cost[i - 1][j] = double(weight[j]) / 
                              (double(terminals.size()) - 1.0);
      }
   }
   for (int i = 2; i < terminals.size(); i++) {
      for (int j = 0; j < m; j++) {
         lag_cost[i - 1][j] += mul[i - 2][j];
         lag_cost[0][j] -= mul[i - 2][j];
      }
   }
}

double Relaxation::solveSubproblem() {
   double obj = 0.0;
   for (int i = 1; i < terminals.size(); i++) {
      obj += minCostToConnect(lag_cost[i - 1], terminals[0], terminals[i],
                              sol[i - 1]);
   }
   return obj;
}

double Relaxation::computeSubgradient() {
   double norm = 0.0;
   for (int i = 2; i < terminals.size(); i++) {
      for (int j = 0; j < m; j++) {
         sg[i - 2][j] = sol[i - 1][j] - sol[0][j]; 
         norm += sg[i - 2][j] * sg[i - 2][j];
      }
   }
   cout << "norm = " << norm << endl;
   return norm;
}

void Relaxation::run() {
   double best_obj = 0.0;
   double step_mul = 0.1;
   int iwi = 0;
   for (int iteration = 0; iteration < 1000; iteration++) {
      computeLagrangianCost();
      double obj = solveSubproblem();
      if (obj > best_obj) {
         best_obj = obj;
         iwi = 0;
         step_mul *= 1.1;
      } else {
         iwi++;
         if (iwi > 5) {
            step_mul *= 0.9;
            iwi = 0;
         }
      }

      cerr << "[" << obj << ", " << best_obj << "]" << endl;

      double norm = computeSubgradient();
      if (norm < 1.e-5) break;
      double ub = 41400;
      double step = step_mul * (ub - best_obj) / norm;
      for (int i = 2; i < terminals.size(); i++) {
         for (int j = 0; j < m; j++) {
            mul[i - 2][j] += step * sg[i - 2][j];
         }
      }
   }
}
   
Relaxation::Relaxation(const vector<vector<pair<int, int>>>& graph, const vector<bool>& is_terminal) {
   g = graph;
   n = g.size();
   m = 0;
   for (int i = 0; i < n; i++) {
      for (auto& e : g[i]) {
         if (i < e.first) {
            weight.push_back(e.second);
            e.second = edges.size();
            edges.push_back({i, e.first});
            m++;
         } else {
            e.second = distance(edges.begin(), lower_bound(edges.begin(), edges.end(), pair<int, int>(e.first, i)));
         }
      }
   }
   for (int i = 0; i < n; i++) {
      if (is_terminal[i]) {
         terminals.push_back(i);
      }
   }

   fixed_to_zero = vector<int>(m);
   fixed_to_one = vector<int>(m);
   dst = vector<double>(n);
   edge_to_parent = vector<int>(n);

   if (terminals.size() < 3) return;

   lag_cost = vector<vector<double>>(terminals.size() - 1, vector<double>(m));
   sol = vector<vector<int>>(terminals.size() - 1, vector<int>(m));
   mul = vector<vector<double>>(terminals.size() - 2, vector<double>(m));
   sg = vector<vector<double>>(terminals.size() - 2, vector<double>(m));

   cout << n << ' ' << m << endl;
}
