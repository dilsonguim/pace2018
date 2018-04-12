#include "order_optimizer.h"

int OrderOptimizer::Solve(int i) {
  if(dp[i] != -1) {
    return dp[i];  
  }
  if((*tree)[i].empty()) {
    dp[i] = 1;
    return dp[i];
  } 
  int u = (*tree)[i][0];
  if((*tree)[i].size() == 1) {
    dp[i] = max(2, Solve(u));  
    return dp[i];
  }
  int v = (*tree)[i][1];
  if(Solve(u) == Solve(v)) {
    dp[i] = max(3, dp[u]+1);  
  }
  else {
    dp[i] = max(3, max(Solve(u), Solve(v)));
  }
  return dp[i];
}

void OrderOptimizer::Optimize(int i) {
  for(auto& c : (*tree)[i]) {
    Optimize(c);  
  }
  if((*tree)[i].size() == 2 && dp[(*tree)[i][1]] > dp[(*tree)[i][0]]) {
    swap((*tree)[i][0], (*tree)[i][1]); 
  }
}
