#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>

using namespace std;


int main(int argc, char** argv) {
  int n = atoi(argv[1]);
  double pe = atof(argv[2]);
  double pt = atof(argv[3]);
  srand48(atol(argv[4]));


  vector<pair<pair<int, int>, int>> edges;
  vector<int> terminals;

  for (int i = 1; i <= n; i++) {
    if (pt > drand48()) terminals.push_back(i);
    for (int j = i + 1; j <= n; j++) {
      if (pe > drand48()) {
        edges.push_back({{i, j}, lrand48() % 100});
      }
    }
  }

  cout << "SECTION Graph" << endl;
  cout << "Nodes " << n << endl;
  cout << "Edges " << edges.size() << endl;
  for (auto& e : edges) {
    cout << "E " << e.first.first << ' ' << e.first.second << ' ' << e.second << endl;
  }
  cout << "END" << endl;

  cout << "SECTION Terminals" << endl;
  cout << "Terminals " << terminals.size() << endl;
  for (auto t : terminals) {
    cout << "T " << t << endl;
  }
  cout << "END" << endl;

  cerr << "p tw " << n << ' ' << edges.size() << endl;
  for (auto& e : edges) {
    cerr << e.first.first << ' ' << e.first.second << endl;
  }
}
