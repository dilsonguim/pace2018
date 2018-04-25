#include "relaxation2.h"

Relaxation2::Relaxation2(const vector<vector<pair<int, int>>>& graph, const vector<bool>& is_terminal) {
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

   fixed_at_zero = vector<int>(m);
   fixed_at_one = vector<int>(m);

   mul = vector<pair<double, double>>(m);
   sg = vector<pair<double, double>>(m);
   lag_cost_x = vector<double>(m);
   lag_cost_y = vector<double>(n);
   sol_x = vector<int>(m);
   sol_y = vector<int>(n);
}
   
void Relaxation2::ComputeLagrangianCost() {
   fill(lag_cost_y.begin(), lag_cost_y.end(), 0.0);
   for (int i = 0; i < m; i++) {
      lag_cost_x[i] = weight[i] + mul[i].first + mul[i].second;
      lag_cost_y[edges[i].first] -= mul[i].first;
      lag_cost_y[edges[i].second] -= mul[i].second;
   }
}

double Relaxation2::SolveSubproblem() {
   vector<pair<double, int>> order_x(m);
   for (int i = 0; i < m; i++) {
      order_x[i].first = lag_cost_x[i];
      order_x[i].second = i;
   }
   sort(order_x.begin(), order_x.end());

   vector<pair<double, int>> order_y(n);
   for (int i = 0; i < n; i++) {
      order_y[i].first = lag_cost_y[i];
      order_y[i].second = i;
      if (binary_search(terminals.begin(), terminals.end(), i)) {
         order_y[i].first = 1.e16;
      }
   }
   sort(order_y.begin(), order_y.end());
   
   double value = 0.0;
   double ans = numeric_limits<double>::infinity();

   DSF dsf(n);
   int count = 0;
   int i_best = 0;
   for (int id = 0; id < m; id++) {
      if (fixed_at_one[id]) {
         if (dsf.Find(edges[id].first) == dsf.Find(edges[id].second)) {
            return numeric_limits<double>::infinity();  
         };
         dsf.Join(edges[id].first, edges[id].second);
         value += lag_cost_x[id];
         count++;
      }
   }
   for (int i = 0; i < m; i++) {
      int id = order_x[i].second;

      if (fixed_at_one[id] or fixed_at_zero[id]) continue;

      if (dsf.Find(edges[id].first) == dsf.Find(edges[id].second)) continue;
      dsf.Join(edges[id].first, edges[id].second);
      value += lag_cost_x[id];
      count++;

      if (count + 1 < terminals.size()) continue;

      if (count >= terminals.size()) {
         value += lag_cost_y[order_y[count - terminals.size()].second];
      }

      if (value < ans) {
         ans = value;
         i_best = i;
      }
   }

   fill(sol_x.begin(), sol_x.end(), 0);
   fill(sol_y.begin(), sol_y.end(), 0);
   dsf = DSF(n);
   count = 0;
   for (int id = 0; id < m; id++) {
      if (fixed_at_one[id]) {
         if (dsf.Find(edges[id].first) == dsf.Find(edges[id].second)) continue; 
         dsf.Join(edges[id].first, edges[id].second);
         sol_x[id] = 1;
         count++;
      }
   }
 
   for (int i = 0; i <= i_best; i++) {
      int id = order_x[i].second;
      if (fixed_at_one[id] or fixed_at_zero[id]) continue;
      if (dsf.Find(edges[id].first) == dsf.Find(edges[id].second)) continue;
      dsf.Join(edges[id].first, edges[id].second);
      sol_x[id] = 1;
      count++;

      if (count + 1 < terminals.size()) continue;

      if (count >= terminals.size()) {
         sol_y[order_y[count - terminals.size()].second] = 1;
      }
   }

   return ans;
}

double Relaxation2::ComputeSubgradient() {
   double norm = 0.0;
   for (int i = 0; i < m; i++) {
      sg[i].first = sol_x[i] - sol_y[edges[i].first];
      sg[i].second = sol_x[i] - sol_y[edges[i].second];

      norm += sg[i].first * sg[i].first;
      norm += sg[i].second * sg[i].second;
   }

   return norm;
}

void Relaxation2::Run() {
   double step_multiplier = 1.99;
   double max_obj = -numeric_limits<double>::infinity();
   int iwi = 0;
   int gi = 0;
   for (int iteration = 0; iteration < 100000; iteration++) {
      ComputeLagrangianCost();
      double obj = SolveSubproblem();

      if (obj > max_obj) {
         max_obj = obj;
         iwi = 0;
         gi++;
         if (gi > 3) {
            step_multiplier = min(step_multiplier * 1.1, 1.99);
            gi = 0;
         }
      } else {
         iwi++;
         gi = 0;
         if (iwi > 20) {
            step_multiplier *= 0.99;
            iwi = 0;
         }
      }
      
      cout << obj << " " << max_obj << endl;

      double norm = ComputeSubgradient();
      if (norm < 1.e-5) {
         cout << "Bom!" << endl;
         break;
      }
      double ub = 1.1 * (max_obj + 1.0); 
      double step = step_multiplier * (ub - obj) / norm;
      for (int i = 0; i < m; i++) {
         mul[i].first = max(0.0, mul[i].first + step * sg[i].first);
         mul[i].second = max(0.0, mul[i].second + step * sg[i].second);

         if (binary_search(terminals.begin(), terminals.end(), edges[i].first)) {
            mul[i].first = 0.0;
         }
         if (binary_search(terminals.begin(), terminals.end(), edges[i].second)) {
            mul[i].second = 0.0;
         }
      }
   }

   for (int i = 0; i < n; i++) {
      for (auto& e : g[i]) {
         fixed_at_zero[e.second]++;
      }
      
      cout << "Fix(" << i << ") = " << SolveSubproblem() << endl;

      for (auto& e : g[i]) {
         fixed_at_zero[e.second]--;
      }
   }
}

DSF::DSF(int n) {
   p = vector<int>(n);
   for (int i = 0; i < n; i++) p[i] = i;
}


int DSF::Find(int x) {
   if (p[x] != x) {
      p[x] = Find(p[x]);
   }
   return p[x];
}


void DSF::Join(int x, int y) {
   int px = Find(x);
   int py = Find(y);
   p[px] = py;
}

