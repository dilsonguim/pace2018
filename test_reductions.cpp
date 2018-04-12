#include "instance.h"
#include "input_parser.h"
#include "reductions.h"

#include <iostream>

using namespace std;


int main(int argc, char* argv[]) {
   Instance* instance = parseInput(cin);
   vector<Edge> sol = reduceAndSolve(instance);
   long long value = 0;
   for (const auto& e : sol) {
      value += e.weight;
   }
   cout << "VALUE " << value << endl;
   for (const auto& e : sol) {
      cout << (e.endpoints[0] + 1) << ' ' << (e.endpoints[1] + 1) << endl;
   }
   delete instance;
}
