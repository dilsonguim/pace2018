#include "instance.h"
#include "input_parser.h"
#include "reductions.h"

#include <iostream>

using namespace std;


int main(int argc, char* argv[]) {
   Instance* instance = parseInput(cin);
   cout << "Original n = " << instance->graph.nodes.size() << 
        " m = " << instance->graph.edges.size() << endl;
   reduceAndSolve(instance);
}
